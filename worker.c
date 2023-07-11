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
        printf("USAGE: ./public.out <server name>\n");
        printf("Because no server is present, using localhost\n");
    }

    int port = 8888;
    char server_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';
    sprintf(server_addr, "tcp://%s:%d", argv[1], port);
    printf("(public, server) = (%s, %s)\n", host, server_addr);
    void *context=zmq_ctx_new();
    void* public = connect_socket(context, server_addr);
    //check if socket returned NULL
    if (public == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(public);
    	zmq_ctx_destroy(context);
    	
    }

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

        sprintf(str, "public;checkin;%s;%s;%li;%f;%f;%f;%f", argv[1],host, numCores, freeMem, upTime, memInUse, loadAvg);

        s_send(public,str);

        strcpy(recvbuffer, s_recv(public));
        printf("%s\n",recvbuffer);

        zmq_close(public);


    //Binds socket to port
    port = 8888;
    context = zmq_ctx_new();
    void *public = zmq_socket(context, ZMQ_REQ);
    
    // int buffer_size = 1024 * 10;
	// zmq_setsockopt(public, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));

    rc = zmq_bind(public, "tcp://*:8888");
    if(rc != 0) {
        perror("Unable to bind to port\n");
        exit(1);
    }

    // s_send(public, "Waiting for work");

    strcpy(recvbuffer, s_recv(public));
    printf("%s\n", recvbuffer);
    
    
    //Closes socket and context
    zmq_close(public);
    zmq_ctx_destroy(context);
    return 0;
}
