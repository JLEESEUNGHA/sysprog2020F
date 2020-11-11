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

#define BUF_LEN 256

int test_thread(void *test_data);

int main(int argc, char *argv[]) {
    char buf[BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    int client_fd;
    int n_port, target_port[BUF_LEN];
    

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
        sscanf(argv[i + 2], "%d", &target_port[i]);
    }


    // create socket(s)
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Client: Can't open stream socket!\n");
        return -1;
    }

    // initialise server_addr 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1111);
    server_addr.sin_addr.s_addr = inet_addr("192.168.56.101");

    // attempt connection(s)
    ///*
    if (connect(client_fd,
                (struct sockaddr*) &server_addr,
                sizeof(server_addr)) < 0) {
        printf("Connection failed.\n");
        return -1;
    }
    while (1) {
        if (read(client_fd, buf, 0) == 0) {
            continue;
        }
        printf("%s", buf);
        if (strcmp(buf, "@@@@@")) {
            break;
        }
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

}

int test_thread(void *test_data) {
    printf("%s", ((char *) test_data));
    return 0;
}
