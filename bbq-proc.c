#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#include <bbq.h>

/* get external definitions */
extern BBQ g_bbq;

/* set maximum message length */
#define MAX_BBQ_MSG_LEN 200


/* define proc files */
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;


/* define proc file operations */
static int bbq_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Opening BBQ probe...\n");
    return 0;
}

static ssize_t bbq_read(struct file *file,
                        char __user *user_buffer,
                        size_t len,
                        loff_t *offset) {
    
    // loop until the end of the queue is reached.
    static unsigned int curr_bbq_pos = 0;
    static unsigned int finished = 1;
    int len_message = 0;

    if (finished) {
        printk(KERN_INFO "Reading BBQ...\n");
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
            printk(KERN_INFO "BBQ: Out of buffer space. Stopping read operation...\n");
            return 0;
        }
        int cpyf_len = copy_to_user(user_buffer, message, len_message);
        if (cpyf_len) {
	    printk(KERN_ERR "BBQ: Error while copying data to user space. Stopping read operation...\n");
	    return 0;
        }

    } else {
	curr_bbq_pos = g_bbq.head;
        finished = 1;
        printk(KERN_INFO "Finished reading BBQ.\n");
    }
     
    return len_message;
}

static ssize_t bbq_write(struct file *file,
                         const char __user *user_buffer,
                         size_t count,
                         loff_t *ppos) {
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
    .read = bbq_read,
    .write = bbq_write,
    .open = bbq_open
};


/* define module initiation and exit sequences*/
static int __init bbq_proc_init(void) {
    proc_dir = proc_mkdir("BBQ", NULL);
    proc_file = proc_create("BBQ", 0600, proc_dir, &proc_ops);
    printk(KERN_INFO "BBQ probe init successful.\n");

    return 0;
}

static void  __exit bbq_proc_exit(void) {
    printk(KERN_INFO "Exiting BBQ probe...\n");
    return;
}


/* misc. */
module_init(bbq_proc_init);
module_exit(bbq_proc_exit);

MODULE_AUTHOR("Lee Seung Ha");
MODULE_DESCRIPTION("BBQ /proc writer");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");
