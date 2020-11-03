/*
 * System Programming Fall 2020 Assignment 1:
 * Simple Circular Queue
 */


typedef struct bio_blob_o_write_info {
	sector_t sector_addr;	//u64?
	char *device_name;
	ktime_t time;		//s64?	

} BB;


#define MAX_BBQ_LEN 50000 //one greater than the actual greatest possible size of the queue.
typedef struct bbqueue {
	BB *queue;
	unsigned int head;
	unsigned int next_pos;
} BBQ;


