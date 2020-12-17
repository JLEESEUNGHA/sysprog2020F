#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#include <linux/skbuff.h>
#include <linux/netdevice.>
#include <linux/ip.h>
#include <linux/tcp.h>

static struct nf_hook_ops spa3_hook_ops;

/* define drop function STUB */
int in_drop_list(int ip, int portno) {
	return 1;
}


/* define hook function */
static unsigned int spa3_hook(unsigned int no,
				struct sk_buff *skb,
				const struct netdev_in,
				const struct netdev_out,
				int (*okfn)(struct sk_buff)) {
	
	if (!skb)
		return NF_ACCEPT;
	else {
		ip_header = (struct iphdr*) skb_network_header(skb);
		if (!ip_header)
			return NF_ACCEPT;
		else {
			if (ip_header->protocol == TCP) {
				if (in_drop_list(0, 0)) { //put in ip_addr and tcp portno
					return NF_DROP;
				}
				else {
					return NF_ACCEPT;
				}
			}
		}
	}
}

/* define proc files */
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;


/* define proc file operations */
static int spa3_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Opening spa3...\n");
    return 0;
}

static ssize_t spa3_read(struct file *file,
                        char __user *user_buffer,
                        size_t len,
                        loff_t *offset) {

    /* REPLACE WITH USER I/O TO GET FIREWALL RULES */
    
    // loop until the end of the queue is reached.
    static unsigned int curr_bbq_pos = 0;
    static unsigned int finished = 1;
    int len_message = 0;

    // check if the queue has been fully read.
    if (finished) {
        curr_bbq_pos = g_bbq.head;
        finished = 0;
    }

    if (curr_bbq_pos != g_bbq.next_pos) {
        char message[MAX_BBQ_MSG_LEN];
        BB blob = g_bbq.queue[curr_bbq_pos];
	curr_bbq_pos = (curr_bbq_pos + 1) % MAX_BBQ_LEN;

        // generate message string
        len_message = sprintf(message, "sector_addr: %d | device_name: %s | time: %u\n" ,
                        blob.sector_addr, blob.device_name, blob.time);

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
	curr_bbq_pos = g_bbq.head;
        finished = 1;
        printk(KERN_INFO "Finished reading spa3.\n");
    }
     
    return len_message;
}

static ssize_t spa3_write(struct file *file,
                         const char __user *user_buffer,
                         size_t count,
                         loff_t *ppos) {

    /* REPLACE WITH USER I/O TO SET FIREWALL RULES */



    char buf[6];
    copy_from_user(buf, user_buffer, 5);
    buf[5] = '\0';
    if (strcmp("clear", buf) == 0) {
        printk(KERN_INFO "Clearing BBQ...\n");
        // the data inside the queue is left untouched.
        g_bbq.head = 0;
        g_bbq.next_pos = 0;
    } else {
        printk(KERN_INFO "Input \"clear\" into BBQ to reset the queue.\n");
    }

    return count;
}



/* define fileproc_ops */
static const struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = spa3_read,
    .write = spa3_write,
    .open = spa3_open
};


/* define module initiation and exit sequences*/
static int __init spa3_proc_init(void) {

	spa3_hook_ops.hook 	= spa3_hook;
	spa3_hook_ops.hooknum	= 1;
	spae_hook_ops.pf	= PF_INET;
	nf_register_hook(&spa3_hook_ops);


    proc_dir = proc_mkdir("spa3", NULL);
    proc_file = proc_create("spa3", 0600, proc_dir, &proc_ops);
    printk(KERN_INFO "spa3 init successful.\n");

    return 0;
}

static void  __exit spa3_proc_exit(void) {
	nf_unregister_hook(&spa3_hook_ops);
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
