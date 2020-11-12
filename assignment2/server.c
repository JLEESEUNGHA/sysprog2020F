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

#define BUF_LEN 2048 // max msg size approx. 1500

void * test_thread(void *test_data);
int create_connection_thread(void *);

int main(int argc, char *argv[]) {
    unsigned short n_port, target_port[10];
    

    // read the number of ports to create
    if (argv[1] == NULL) {
        printf("Please provide the number of ports to create.\n");
        return 0;
    }
    n_port = atoi(argv[1]);
    //sscanf(argv[1], "%u", &n_port);
    if (n_port > 10) {
        printf("Maximum of 10 ports allowed.\nSetting the number of ports to 10...\n");
        n_port = 10;
    }
    //printf("Number of ports specified: %d\n", n_port);

    // save port numbers
    if (argc < n_port + 2) {
            printf("Insufficient number of ports specified.\n");
            return 0;
    }
    for (int i = 0; i < n_port; ++i) {
        //printf("Port No.:%s\n", argv[i + 2]);
        target_port[i] = atoi(argv[i + 2]);
        //sscanf(argv[i + 2], "%u", &target_port[i]); <- doing this causes target_port to be interpreted as a char array, causes n_port to be overwritten with a 0.
    }


    ///*
    // create threads
    pthread_t p_thread[10];
    int th_id[10];
    int th_stat[10];
    void *thread_func = &create_connection_thread;

    for (int i = 0; i < n_port; ++i) {
        unsigned short *data = &target_port[0];
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
        printf("th_stat[%d]: %d\n", i, th_stat[i]);
    }
    //*/

    return 0;
}


int create_connection_thread(void *args) {
    char buf[BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    int server_fd;
    unsigned short target_port;

    if (args == NULL) {
        printf("Port number not specified.\n");
        return -1;
    }
    target_port = ((unsigned short *) args)[0];

    // create socket(s)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Server: Can't open stream socket!\n");
        return -1;
    }

    // initialise server_addr 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    server_addr.sin_addr.s_addr = htonl("127.0.0.1");//INADDR_ANY);    // hard-coded address
    printf("%s", server_addr.sin_addr.s_addr);

    // attempt to create connection(s)
    ///*
    int berrc;
    if ((berrc = bind(server_fd,
                (struct sockaddr*) &server_addr,
                sizeof(server_addr))) < 0) {
        printf("Binding failed. Code: %d\n", berrc);
        return -1;
    }
    
    // receive data from server
    int count, pos, break_flag = 0;
    char filename[BUF_LEN];
    sscanf(filename, "%ud-%ud.txt", target_port, server_fd);
    /*strcat(filename, itoa(target_port));
    strcat(filename, "-");
    strcat(filename, itoa(server_fd));
    strcat(filename, ".txt");*/
    FILE *file = fopen(filename, "a+");

    if (listen(server_fd, BUF_LEN) < 0) {
        printf("Error while attemping to listen.\n");
        return 0;
    }

    ///*
    // listen to client requests
    while (1) {
        int cl_struct_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &cl_struct_len);
        
        if (client_fd < 0) {
            printf("Failed to accept connection.\n");
            return 0;
        }

        // read from client
        if ((count = read(server_fd, buf, BUF_LEN - 1)) == 0) {
            printf("No more characters to read.");
            break;
        }
        
        for (int i = 0; i < 10; ++i);
            write(client_fd, "test_msg", BUF_LEN);
        write(client_fd, "@@@@@", BUF_LEN);
        /*
        if (client_fd % 2 == 0)
            write(client_fd, "test_msg", BUF_LEN);
        //*/
        close(client_fd);
        /*
        if (pos = strstr(buf, "@@@@@") != NULL) {
            buf[pos + 4] = '\0';
            break_flag = 1;
        }

        // write to file
        fprintf(file, "<time>|%d|%s\n", count, buf);
        if (break_flag) {
            break;
        }
        //*/
    }
    //*/

    // close connection
    if (close(server_fd)) {
        printf("Error closing file.\n");
        return -1;
    }
    //*/
    return 0;
}


void * test_thread(void *test_data) {
    printf("%s", ((char *) test_data));
}
