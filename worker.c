#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include "sysdata.h"
#include "zmq_msgs.h"

#define MAXLEN 512

void do_work(char *input, char *output);

int main(int argc, char** argv) {
    //Binds socket to port
    if(argc < 2) {
        printf("USAGE: ./worker.out <server name>\n");
        printf("Because no server is present, using localhost\n");
    }
    // int port = 5555;
    // char server_addr[55];
    // char host[55];
    // gethostname(host, sizeof(host));
    // host[55 - 1] = '\0';
    // sprintf(server_addr, "tcp://%s:%d", (argc < 2) ? "localhost" : argv[1], port);
    // printf("(worker, server) = (%s, %s)\n", host, server_addr);
    // // Pair pattern from worker to server/public
    // void *context = zmq_ctx_new();
    // void *public = zmq_socket(context, ZMQ_PAIR);

    // int buffer_size = 1024 * 10;
	// zmq_setsockopt(public, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));
	
	// //attempt to connect socket to provided server
	// int rc = zmq_connect(public, server_addr);

    void *context = zmq_ctx_new();
    if(context == NULL) {
        perror("Could not create zmq context\n");
        exit(1);
    }
    void *public = zmq_socket (context, ZMQ_PAIR);
    if(public == NULL) {
        perror("Could not create public socket\n");
        exit(1);
    }

    int buffer_size = 1024 * 10;
	zmq_setsockopt(public, ZMQ_RCVBUF, &buffer_size, sizeof(buffer_size));

    int rc = zmq_bind(public, "tcp://*:8888");
    if(rc != 0) {
        perror("Could not bind\n");
        zmq_close(public);
        zmq_ctx_destroy(context);
        exit(1);
    }

    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';

    //Calculates system details to send to public
        float upTime = 0;
        float loadAvg = 0;
        float memInUse;
        float totalMem;
        float freeMem;
        char str [MAXLEN];
        char recvbuffer[MAXLEN];

        getMemoryDetail(&memInUse, &totalMem, &freeMem);
        long numCores = sysconf(_SC_NPROCESSORS_ONLN);

        sprintf(str, "worker;checkin;%s;%s;%li;%f;%f;%f;%f", argv[1],host, numCores, freeMem, upTime, memInUse, loadAvg);

        s_send(public,str);

        strcpy(recvbuffer, s_recv(public));
        printf("%s\n",recvbuffer);

        zmq_close(public);


    //Binds socket to port
    int port = 8888;
    context = zmq_ctx_new();
    void *worker = zmq_socket(context, ZMQ_PAIR);
    
    buffer_size = 1024 * 10;
	zmq_setsockopt(worker, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));

    rc = zmq_bind(worker, "tcp://*:8888");
    if(rc != 0) {
        perror("Unable to bind to port\n");
        exit(1);
    }

    s_send(worker, "Waiting for work");

    strcpy(recvbuffer, s_recv(worker));
    printf("%s\n", recvbuffer);
    
    
    //Closes socket and context
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return 0;
}
