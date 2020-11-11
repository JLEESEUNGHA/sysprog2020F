#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifdef _WIN32 // for development on Windows (thru MinGW)
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
#endif

#define BUF_LEN 2048

int test_thread(void *test_data);

int main(int argc, char *argv[]) {
    char buf[BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    int client_fd, n_port;
    unsigned short target_port[BUF_LEN];
    

    // read the number of ports to create
    if (argv[1] == NULL) {
        printf("Please provide the number of ports to create.\n");
        return 0;
    }
    sscanf(argv[1], "%d", &n_port);
    //printf("%d\n", n_port);

    // save port addresses
    for (int i = 0; i < n_port; ++i) {
        //printf("%s\n", argv[i + 2]);
        if (argv[i + 2] == NULL) {
            printf("Not enough ports specified.\n");
            return 0;
        }
        sscanf(argv[i + 2], "%u", &target_port[0]);
    }


    // create socket(s)
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Client: Can't open stream socket!\n");
        return -1;
    }

    // initialise server_addr 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port[0]);
    server_addr.sin_addr.s_addr = inet_addr("192.168.56.101");

    // attempt connection(s)
    ///*
    if (connect(client_fd,
                (struct sockaddr*) &server_addr,
                sizeof(server_addr)) < 0) {
        printf("Connection failed.\n");
        return -1;
    }


    // receive data from server
    int count, pos, break_flag = 0;
    FILE *file = fopen("datalog", "a+");

    while (1) {
        if ((count = read(client_fd, buf, BUF_LEN - 1)) == 0) {
            printf("No more characters to read.");
            break;
        }
        if (pos = strstr(buf, "@@@@@") != NULL) {
            buf[pos + 4] = '\0';
            break_flag = 1;
        }

        // write to file
        fprintf(file, "<time>|%d|%s\n", count, buf);
        if (break_flag) {
            break;
        }
    }

    // close connection
    if (close(client_fd)) {
        printf("Error closing file.\n");
        return -1;
    }
    //*/


    // test threads
    ///*
    pthread_t p_thread;
    int th_id;
    int status;
    char data[] = "1234";
    if (th_id = pthread_create(&p_thread, NULL, test_thread, (void *) data) < 0) {
        printf("Error while creating thread.\n");
        return -1;
    }
    if (pthread_join(p_thread, (void **) &status)) {
        printf("Error in thread.\n");
        return -1;
    }
    //*/
    return 0;
}

int test_thread(void *test_data) {
    printf("%s", ((char *) test_data));
    return 0;
}
