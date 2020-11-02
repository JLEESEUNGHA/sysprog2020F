#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include <block/blk-core.c>


/* get external definitions */
extern MAX_BBQ_LEN;
extern BBQ;
extern g_bbq;

/* set maximum message length */
#define MAX_BBQ_MSG_LEN 1000


/* define proc files */
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;


/* define proc file operations */
static int bbq_open() {
    printk(KERN_INFO "Opening BBQ probe...\n");
    return 0;
}

static ssize_t bbq_read(struct file *file,
                        char __user *user_buffer,
                        size_t len,
                        loff_t *offset) {
    printk(KERN_INFO "Reading BBQ...\n");
    unsigned long total_len = 0; // the total number of bytes copied to the user buffer.

    // loop until the end of the queue is reached.
    unsigned int curr_bbq_pos = g_bbq.head;
    while (curr_bbq_pos != g_bbq.next_pos) {
        char message[MAX_BBQ_MSG_LEN];
        BB blob = g_bbq.queue[curr_bbq_pos++];

        // generate message string
        len_message = sprintf(message, "sector_addr: %d | device_name: %s | time: %d" ,
                        blob.sector_addr, blob.device_name, blob.time);

        // copy string to user buffer
        if (total_len + len_message > len) {
            printk(KERN_INFO "BBQ: Out of buffer space. Stopping read operation...\n");
            break;
        }
        if (!user_buffer += copy_to_user(user_buffer, message, len_message) {
            printk(KERN_ERR "BBQ: Error while copying data to user space. Stopping read operation...\n");
            break;
        }
        total_len += len_message;
    }
        

    return ;
}

static ssize_t bbq_write(struct file *file,
                         const char __user *user_buffer,
                         size_t count,
                         loff_t *ppos) {
    if (strcmp("clear", user_buffer)) {
        printk(KERN_INFO "Clearing BBQ...\n");
        // the data inside the queue is left untouched.
        g_bbq.head = 0;
        g_bbq.next_pos = 0;
    } else {
        // (might want to remove user_buffer to prevent the user from overflowing the buffer in an actual release.)
        printk(KERN_INFO "BBQ command %s unrecognised...\n", user_buffer);
        
    }

    return count;
}



/* define fileproc_ops */
static const struct file_operations proc_ops = {
    .owner = THIS_MODULE;
    .read = bbq_read();
    .write = bbq_write();
    .open = bbq_open();
}


/* define module initiation and exit sequences*/
static int __init bbq_proc_init() {
    proc_dir = proc_mkdir("BBQ", NULL);
    proc_file = proc_create("BBQ", 0600, proc_dir, &proc_ops);
    printk(KERN_INFO "BBQ probe init successful.\n");

    return 0;
}

static void __exit bbq_proc_exit() {
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