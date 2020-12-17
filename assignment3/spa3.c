#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/tcp.h>

// change as appropriate
#define MAX_RULE_LIST_LEN 1000
#define MAX_MSG_LEN 1000
#define TEST_PORT 1111

#define INBOUND		0
#define OUTBOUND	1
#define FORWARD		2
#define PROXY 		3
#define DROP_INBOUND	4 
#define DROP_OUTBOUND	5
#define DROP_FORWARD	6
#define ACCEPT		7


// define nf_hook_ops
static struct nf_hook_ops spa3_hook_pre;
static struct nf_hook_ops spa3_hook_forward;
static struct nf_hook_ops spa3_hook_post;

// define some global variables
static int rule_list[MAX_RULE_LIST_LEN][2]; //[0]:port, [1]:type
static int rule_list_len = 0;
const char* op_msg[8] = {
	"INBOUND",
	"OUTBOUND",
	"FORWARD",
	"PROXY(INBOUND)",
	"DROP(INBOUND)",
	"DROP(OUTBOUND)",
	"DROP(FORWARD)",
	"ACCEPT"
};
 
// define function to check whether a packet should be dropped
int is_drop(int op_id) {
    if (op_id == DROP_INBOUND || op_id == DROP_OUTBOUND || op_id == DROP_FORWARD) {
        return 1;
    } else {
        return 0;
    }
}


/* define rule list check function STUB */
/*
int in_rule_list(int type, unsigned int portno) {
    if (type == INBOUND && portno == TEST_PORT)
        return 1;
    else if (type == OUTBOUND && portno == TEST_PORT + 1111)
        return 1;
    else if (type == FORWARD && portno == TEST_PORT + 3333)
        return 1;
    else if (type == PROXY && portno == TEST_PORT + 3333)
        return 1;
    return 0;
}
*/

/* define rule list check function */
///*
int in_rule_list(int type, unsigned int portno) {
    int i = 0;
    for (; i < rule_list_len; ++i) {
        if (rule_list[i][0] == portno && rule_list[i][1] == type)
            return 1;
    }
    return 0;
}
//*/


/* define hook functions */
int __spa3_hook(struct sk_buff *skb, int type) {
    const unsigned int redirect_addr = 0x01010183; //131.1.1.1
    struct iphdr* ip_header = ip_hdr(skb);
    unsigned int protocol = ip_header->protocol;
    unsigned int saddr = ip_header->saddr;
    unsigned int daddr = ip_header->daddr;
    int op_id = 7; //default: generic accept

    //printk("spa3_hook: protocol: %d, saddr: %u, daddr: %u\n", ip_header->protocol, ip_header->saddr, ip_header->daddr); //sanity check.

    // assume all relevant packets are TCP packets.
    if (ip_header->protocol == IPPROTO_TCP) {

        // extract TCP header
        struct tcphdr* tcp_header = (struct tcphdr*)((char*)ip_header + sizeof(struct iphdr));

        // obtain the source and destination port numbers
        unsigned int src_portno = htons(tcp_header->source);
        unsigned int dest_portno = htons(tcp_header->dest);

        // extract relevant bits
        unsigned int syn_bit, fin_bit, ack_bit, rst_bit;
        syn_bit = tcp_header->syn;
        fin_bit = tcp_header->fin;
        ack_bit = tcp_header->ack;
        rst_bit = tcp_header->rst;

        // check what kind of action should be taken on the packet
        switch (type) {
            case INBOUND:
                op_id = in_rule_list(INBOUND, src_portno) == 1 ? DROP_INBOUND : INBOUND;
                if (in_rule_list(PROXY, src_portno)) {
                    op_id = PROXY;

                    // change src and dest addr.
                    ip_header->daddr = redirect_addr;
                    tcp_header->dest = src_portno;

                    // update local vars
                    daddr = ip_header->daddr;
                    dest_portno = tcp_header->dest;
                }
                break;
            case FORWARD:
                op_id = in_rule_list(FORWARD, dest_portno) == 1 ? DROP_FORWARD : FORWARD;
                break;
            case OUTBOUND:
                op_id = in_rule_list(OUTBOUND, dest_portno) == 1 ? DROP_OUTBOUND : OUTBOUND;
                break;
        }

        printk("%-15s: %d, %d, %d, %d.%d.%d.%d, %d.%d.%d.%d, %d%d%d%d\n",
		op_msg[op_id],
		protocol,
		src_portno,
		dest_portno,
		saddr & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff, (saddr >> 24) & 0xff,
		daddr & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff, (daddr >> 24) & 0xff,
		syn_bit, fin_bit, ack_bit, rst_bit); // TODO: format ip addresses.

    }

    // check whether to drop packet
    if (is_drop(op_id))
        return NF_DROP;
    return NF_ACCEPT;
}


static unsigned int spa3_hpre(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {
    /* pre */
    return __spa3_hook(skb, INBOUND);
}


static unsigned int spa3_hforward(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {
    /* forward */
    return __spa3_hook(skb, FORWARD);
}


static unsigned int spa3_hpost(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {
    /* post */
    return __spa3_hook(skb, OUTBOUND);
}


/* define proc files */
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file_add;
static struct proc_dir_entry *proc_file_show;
static struct proc_dir_entry *proc_file_del;


/* define proc file operations */
static int spa3_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "spa3: Opening file...\n");
    return 0;
}

static ssize_t spa3_add_write(struct file *file,
                         const char __user *user_buffer,
                         size_t count,
                         loff_t *ppos) {

    char buf[8];
    int failed_count = copy_from_user(buf, user_buffer, 7);
    if (failed_count) {
        printk("spa3: Error while reading from user input.");
        count = 0;
        return -1;
    }
    buf[7] = '\0';
    printk("spa3: nRules: %d, buf:%s, buf[n]:%c %d %d %d\n", rule_list_len, buf, buf[0], buf[1], buf[2], buf[3]);

    long int tmp;
    switch (user_buffer[0]) {
        case 'I':
            kstrtol((buf+2), 10, &tmp);
            rule_list[rule_list_len][0] = (int) tmp;
            rule_list[rule_list_len++][1] = INBOUND;

            printk(KERN_INFO "spa3: Inbound rule added\n");
            break;
        case 'O':
            kstrtol((buf+2), 10, &tmp);
            rule_list[rule_list_len][0] = (int) tmp;
            rule_list[rule_list_len++][1] = OUTBOUND;

            printk(KERN_INFO "spa3: Outbound rule added.\n");
            break;
        case 'F':
            kstrtol((buf+2), 10, &tmp);
            rule_list[rule_list_len][0] = (int) tmp;
            rule_list[rule_list_len++][1] = FORWARD;

            printk(KERN_INFO "spa3: Forwarding rule added.\n");
            break;
        case 'P':
            kstrtol((buf+2), 10, &tmp);
            rule_list[rule_list_len][0] = (int) tmp;
            rule_list[rule_list_len++][1] = PROXY;

            printk(KERN_INFO "spa3: Proxy rule added.\n");

            break;
        default:
            printk(KERN_INFO "spa3: Input invalid.\n");
    }

    count = 7;
    return 7;
}

static ssize_t spa3_show_read(struct file *file,
                        char __user *user_buffer,
                        size_t len,
                        loff_t *offset) {

    /* REPLACE WITH USER I/O TO GET FIREWALL RULES */
    /* currently just reads the rule_list, which is always empty */
    
    // loop until the end of the list is reached.
    static unsigned int curr_pos = 0;
    static unsigned int finished = 1;
    int len_message = 0;

    // check if the list has been fully read.
    if (finished) {
        curr_pos = 0;
        finished = 0;
    }

    if (curr_pos < rule_list_len) {
        char message[MAX_MSG_LEN];
        int portno = rule_list[curr_pos][0];
        char type_char;
        switch (rule_list[curr_pos][1]) {
            case 0:
                type_char = 'I';
                break;
            case 1:
                type_char = 'O';
                break;
            case 2:
                type_char = 'F';
                break;
            case 3:
                type_char = 'P';
                break;
        }
        printk("curr_pos: %d, type_char: %c, portno: %d\n", curr_pos, type_char, portno);
	
        // generate message string
        len_message = sprintf(message, "spa3: rule index %d: (%c) %d\n", curr_pos, type_char, portno);
        curr_pos = (curr_pos + 1) % MAX_RULE_LIST_LEN;

        // copy string to user buffer
        if (len_message > len) {
            printk(KERN_INFO "spa3: Out of buffer space. Stopping read operation...\n");
            return 0;
        }
        int cpyf_len = copy_to_user(user_buffer, message, len_message);
        if (cpyf_len) {
	    printk(KERN_ERR "spa3: Error while copying data to user space. Stopping read operation...\n");
	    return 0;
        }

    } else {
	curr_pos = 0;
        finished = 1;
        printk(KERN_INFO "spa3: Finished reading rules.\n");
    }
     
    return len_message;
}

static ssize_t spa3_del_write(struct file *file,
                        char __user *user_buffer,
                        size_t count,
                        loff_t *offset) {

    char buf[2];
    int failed_count = copy_from_user(buf, user_buffer, 1);
    if (failed_count) {
        printk("spa3: Error while reading from user input.");
        count = 0;
        return -1;
    }
    buf[1] = '\0';
    printk("spa3: nRules: %d, buf:%s,", rule_list_len, buf);

    long int index_long;
    kstrtol(buf, 10, &index_long);
    int index = (int) index_long;

    if (rule_list_len > index) {
        // move up all entries after the delete entry
        int i = index;
        for (; i < rule_list_len; ++i) {
            rule_list[i][0] = rule_list[i + 1][0];
            rule_list[i][1] = rule_list[i + 1][1];
        }
        rule_list_len--;
    } else { 
        printk(KERN_INFO "spa3: Input invalid.\n");
    }

    count = 1;
    return 1;
}
 

/* define fileproc_ops */
static const struct file_operations proc_ops_add = {
    .owner = THIS_MODULE,
    .write = spa3_add_write,
    .open = spa3_open
};

static const struct file_operations proc_ops_show = {
    .owner = THIS_MODULE,
    .read = spa3_show_read,
    .open = spa3_open
};

static const struct file_operations proc_ops_del = {
    .owner = THIS_MODULE,
    .write = spa3_del_write,
};


/* define module initiation and exit sequences*/
static int __init spa3_proc_init(void) {

    // register hooks
    spa3_hook_pre.hook 		= spa3_hpre;
    spa3_hook_pre.hooknum	= NF_INET_PRE_ROUTING;
    spa3_hook_pre.pf		= PF_INET;
    nf_register_hook(&spa3_hook_pre);

    spa3_hook_forward.hook 	= spa3_hforward;
    spa3_hook_forward.hooknum	= NF_INET_FORWARD;
    spa3_hook_forward.pf	= PF_INET;
    nf_register_hook(&spa3_hook_forward);

    spa3_hook_post.hook		= spa3_hpost;
    spa3_hook_post.hooknum	= NF_INET_POST_ROUTING;
    spa3_hook_post.pf		= PF_INET;
    nf_register_hook(&spa3_hook_post);


    // create /proc directories and files
    proc_dir = proc_mkdir("group32", NULL);
    proc_file_add = proc_create("add", 0, proc_dir, &proc_ops_add);
    proc_file_show = proc_create("show", 0, proc_dir, &proc_ops_show);
    proc_file_del = proc_create("del", 0, proc_dir, &proc_ops_del);

    printk(KERN_INFO "spa3: init successful.\n");

    return 0;
}

static void  __exit spa3_proc_exit(void) {
    nf_unregister_hook(&spa3_hook_pre);
    nf_unregister_hook(&spa3_hook_forward);
    nf_unregister_hook(&spa3_hook_post);

    printk(KERN_INFO "spa3: Exiting...\n");
    return;
}


/* misc. */
module_init(spa3_proc_init);
module_exit(spa3_proc_exit);

MODULE_AUTHOR("Lee Seung Ha");
MODULE_DESCRIPTION("spa3");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");
