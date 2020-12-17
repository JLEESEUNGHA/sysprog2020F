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
//#include <linux/udp.h>


// change as appropriate
#define SERVER_IP_ADDR 0xc0a83865 //192.168.56.101 //doesn't work, ref to output to fix
#define CLIENT_IP_ADDR 0x0a00020f //10.0.2.15  //doesn't work, ref to output to fix
#define MAX_RULE_LIST_LEN 1000
#define MAX_MSG_LEN 1000
#define TEST_PORT 3333

#define INBOUND		0
#define OUTBOUND	1
#define FORWARD		2
#define PROXY 		3


static struct nf_hook_ops spa3_hook_pre;
static struct nf_hook_ops spa3_hook_forward;
static struct nf_hook_ops spa3_hook_post;

static int rule_list[4][MAX_RULE_LIST_LEN]; //0: Inbound, 1: Outbound, 2: Forward, 3: Proxy
static int rule_list_len[4] = {0};
const char* op_msg[7] = {
	"DROP(INBOUND)",
	"DROP(OUTBOUND)",
	"DROP(FORWARD)",
	"INBOUND",
	"OUTBOUND",
	"FORWARD",
	"PROXY(INBOUND)"
};
 

/* define drop list check function STUB */
int in_rule_list(int type, unsigned int portno) {
    if (type == INBOUND && portno == TEST_PORT)
        return 1;
    else if (type == OUTBOUND && portno == TEST_PORT)
        return 1;
    else if (type == FORWARD && portno == TEST_PORT)
        return 1;
    else if (type == PROXY && portno == TEST_PORT)
        return 1;
    return 0;
}


    /* we need to implement rules for:
     * Inbound: e.g. src is 1111, dest is <whatever> (below).
     * Outbound: e.g. src is <whatever>, dest is 1111.
     * Forward:
     * Proxy:
     * NF_INET_PRE_ROUTING, NF_INET_FORWARD, NF_INET_POST_ROUTING etc.
     * Proxy: change inbound packet's dest. addr. to 131.1.1.1; new dest. port -> src port no (1111)
     */


/* define hook function */
static unsigned int spa3_hook(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {

    struct iphdr* ip_header = ip_hdr(skb);
    unsigned int protocol = ip_header->protocol;
    unsigned int saddr = ip_header->saddr;
    unsigned int daddr = ip_header->daddr;

    // assume it's a TCP header.
    struct tcphdr* tcp_header = (struct tcphdr*)((char*)ip_header + sizeof(struct iphdr));
   int op_id;

    // ignore the next two statements for now (not necessary for assignment)
    //if (ip_header->saddr != SERVER_IP_ADDR || ip_header->daddr != CLIENT_IP_ADDR)
    //    return NF_ACCEPT;
    printk("spa: protocol: %d, saddr: %u, daddr: %u\n", ip_header->protocol, ip_header->saddr, ip_header->daddr);

    // all relevant packets are TCP packets.
    if (ip_header->protocol == IPPROTO_TCP) {

        // obtain the source and destination port numbers
        unsigned int src_portno = htons(tcp_header->source);
        unsigned int dest_portno = htons(tcp_header->dest);
        //printk("spa3: src_portno: %u, dest_portno: %u\n", src_portno, dest_portno); 

        // check if action should be taken on the packet
        if (in_rule_list(0, src_portno)) {
            //printk("spa3: drop cuz src_portno is %d.", src_portno);
            op_id = 0;
        }

        // if not in drop list
        //printk("spa3: inbound %d\n", src_portno);
        op_id = 1;

        // TODO: handle Inbound, Outbound, Forward, Proxy etc.
        printk("%s\t:\t %d, %d, %d, %d, %d\n",
		op_msg[op_id],
		protocol,
		src_portno,
		dest_portno,
		saddr,
		daddr,
		0);//tcp_bits);
        if (op_id)
            return NF_ACCEPT;
    }

    // accept if not TCP
    printk("spa3: protocol not TCP.\n");
    return NF_ACCEPT;
}


static unsigned int spa3_hforward(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {
    /* forward */

    return NF_ACCEPT;
}


static unsigned int spa3_hpost(unsigned int no,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *)) {
    /* post-routing */

    return NF_ACCEPT;
}


/* define proc files */
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file_add;
static struct proc_dir_entry *proc_file_show;
static struct proc_dir_entry *proc_file_del;


/* define proc file operations */
static int spa3_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Opening spa3...\n");
    return 0;
}

static ssize_t spa3_add_write(struct file *file,
                         const char __user *user_buffer,
                         size_t count,
                         loff_t *ppos) {

    /* REPLACE WITH USER I/O TO SET FIREWALL RULES */
    /* currently does next to nothing since we're not adding new rules */


    char buf[6];
    copy_from_user(buf, user_buffer, 5);
    buf[5] = '\0';
    if (strcmp("clear", buf) == 0) {
        printk(KERN_INFO "Clearing rules list...\n");
        rule_list_len[0] = 0;
    } else {
        printk(KERN_INFO "Input \"clear\" to reset the list.\n");
    }

    return count;
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

    // check if the queue has been fully read.
    if (finished) {
        curr_pos = 0;
        finished = 0;
    }

    if (curr_pos < rule_list_len[0]) {
        char message[MAX_MSG_LEN];
        int rule = rule_list[curr_pos];
	curr_pos = (curr_pos + 1) % MAX_RULE_LIST_LEN;

        // generate message string
        len_message = sprintf(message, "RULE: block inbound to port: %d", rule);
        /* TODO: support more things */

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
        printk(KERN_INFO "Finished reading spa3.\n");
    }
     
    return len_message;
}

static ssize_t spa3_del_write(struct file *file,
                        char __user *user_buffer,
                        size_t len,
                        loff_t *offset) {
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
    .open = spa3_open
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
    proc_dir = proc_mkdir("2018320250", NULL);
    proc_file_add = proc_create("add", 0600, proc_dir, &proc_ops_add);
    proc_file_show = proc_create("show", 0600, proc_dir, &proc_ops_show);
    proc_file_del = proc_create("del", 0600, proc_dir, &proc_ops_del);

    printk(KERN_INFO "spa3 init successful.\n");

    return 0;
}

static void  __exit spa3_proc_exit(void) {
    nf_unregister_hook(&spa3_hook_pre);
    nf_unregister_hook(&spa3_hook_forward);
    nf_unregister_hook(&spa3_hook_post);

    printk(KERN_INFO "Exiting spa3...\n");
    return;
}


/* misc. */
module_init(spa3_proc_init);
module_exit(spa3_proc_exit);

MODULE_AUTHOR("Lee Seung Ha");
MODULE_DESCRIPTION("spa3");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");
