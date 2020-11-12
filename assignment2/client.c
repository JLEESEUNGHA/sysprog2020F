#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#ifdef _WIN32   // for development on Windows (thru MinGW)
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
#endif

#define BUF_LEN 4096 // msg size of larger msgs approx. 1500
#define MAX_PORT_NO 65355

void * test_thread(void *test_data);
int create_connection_thread(void *);

int main(int argc, char *argv[]) {
    unsigned short n_port, target_port[10], port_count[MAX_PORT_NO] = {};

    // read the number of ports to create
    if (argc < 2) {
        printf("Please provide the number of ports to create.\n");
        return 0;
    }
    n_port = atoi(argv[1]);
    //sscanf(argv[1], "%u", &n_port);

    // check port limit
    if (n_port > 10) {
        printf("Maximum of 10 ports allowed.\nSetting the number of ports to 10...\n");
        n_port = 10;
    }
    //printf("Number of ports specified: %d\n", n_port);

    // obtain number of ports
    if (argc < n_port + 2) {
            printf("Insufficient number of ports specified.\n");
            return 0;
    }

    // save and count port numbers
    for (int i = 0; i < n_port; ++i) {
        //printf("Port No.:%s\n", argv[i + 2]);
        target_port[i] = atoi(argv[i + 2]);
        //sscanf(argv[i + 2], "%u", &target_port[i]); <- doing this causes target_port to be interpreted as a char array, causes n_port to be overwritten with a 0.
        port_count[target_port[i]]++; // an admittedly memory-inefficient way of counting, but it gets the job done. No bounds checking.
    }

    // check connection limit for each port
    for (int j = 0; j < MAX_PORT_NO; ++j) {
        if (port_count[j] > 5) {
            printf("The maximum number of connections per port is 5.\n");
            return 0;
        }
    }
    //printf("%p ; %p\n", &target_port[10], &n_port);


    ///*
    // create threads
    pthread_t p_thread[10];
    int th_id[10];
    int th_stat[10];
    void *thread_func = &create_connection_thread;

    for (int i = 0; i < n_port; ++i) {
        unsigned short *data = &target_port[i];
        if (th_id[i] = pthread_create(&p_thread[i], NULL, thread_func, (void *) data) < 0) {
            printf("Error while creating thread.\n");
            return -1;
        }
    }

    // wait for threads to finish
    for (int i = 0; i < n_port; ++i) {
        int err;
        if (err = pthread_join(p_thread[i], (void **) &th_stat[i])) {
            printf("Error in thread. pthread_join() returned %d.\n", err);
            return -1;
        }
        //printf("th_stat[%d]: %d\n", i, th_stat[i]);
    }
    //*/

    return 0;
}


int create_connection_thread(void *args) {
    char buf[BUF_LEN];
    struct sockaddr_in server_addr;
    int client_fd;
    unsigned short target_port;

    // check length of args
    if (args == NULL) {
        printf("Port number not specified.\n");
        return -1;
    }

    // obtain target_port
    target_port = ((unsigned short *) args)[0];

    // create socket(s)
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Client: Can't open stream socket!\n");
        return -1;
    }

    // initialise server_addr 
    //memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    server_addr.sin_addr.s_addr = inet_addr("192.168.56.101");    // hard-coded address

    // attempt to create connection(s)
    ///*
    if (connect(client_fd,
                (struct sockaddr*) &server_addr,
                sizeof(server_addr)) < 0) {
        printf("Connection to port %d failed.\n", target_port);
        return -1;
    }
    
    // receive data from server
    int count, break_flag = 0;
    char filename[BUF_LEN];
    sprintf(filename, "%u-%u.txt", target_port, client_fd);
    FILE *file = fopen(filename, "a+");

    while (1) {
        // keep reading from server
        count = 0;
        buf[0] = '\0';
        if ((count = read(client_fd, buf, BUF_LEN - 1)) == 0) {
            printf("No more characters to read.");
            break;
        }
        buf[count] = '\0';

        // check if "@@@@@" is inside the buffer
        char *check_ptr;
        if ((check_ptr = strstr(buf, "@@@@@")) != NULL) {
            //printf("Termination signal detected.\n");
            check_ptr += 5;
            *check_ptr = '\0';
            break_flag = 1;
        }

        static const int FTIME_SIZE = sizeof("HH:MM:SS.MLS");
        struct timespec ts;
        struct tm *timeinfo;
        char time_buf[FTIME_SIZE];
        char msec_buf[3];
        
        clock_gettime(CLOCK_REALTIME, &ts);
        timeinfo = localtime(&ts.tv_nsec);
        
        strftime(time_buf, FTIME_SIZE, "%H:%M:%S", timeinfo);
        sprintf(msec_buf, "%03d", (ts.tv_nsec % 1000000000) / 1000000); // truncate
        strcat(time_buf, msec_buf);

        // write to file
        fprintf(file, "%s|%d|%s\n", time_buf, count, buf);

        // exit loop if break flag is set
        if (break_flag) {
            break;
        }
    }

    // close connection
    if (close(client_fd)) {
        printf("Error closing socket.\n");
        return -1;
    }
    if (fclose(file)) {
        printf("Error closing file.\n");
    }
    //free(&server_addr);
    //*/
    return 0;
}


void * test_thread(void *test_data) {
    printf("%s", ((char *) test_data));
}
