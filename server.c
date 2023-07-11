#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <server.h>

int main(int argc, char** argv) {	
    // Manager is a manager to a client but is also a requester to its workers 
    // void *context = zmq_ctx_new();
    // if(context == NULL) {
    //     perror("Could not create zmq context\n");
    //     exit(1);
    // }
    // void *manager = zmq_socket (context, ZMQ_PAIR);
    // if(manager == NULL) {
    //     perror("Could not create manager socket\n");
    //     exit(1);
    // }

    // int buffer_size = 1024 * 10;
	// zmq_setsockopt(manager, ZMQ_RCVBUF, &buffer_size, sizeof(buffer_size));

    // int rc = zmq_bind(manager, "tcp://*:8888");
    // if(rc != 0) {
    //     perror("Could not bind\n");
    //     zmq_close(manager);
    //     zmq_ctx_destroy(context);
    //     exit(1);
    // }

    void *context = zmq_ctx_new();
    if(context == NULL) {
        perror("Could not create zmq context\n");
        exit(1);
    }
    void *manager = zmq_socket(context, ZMQ_REP);
    if(manager == NULL) {
        perror("Could not create manager socket\n");
        exit(1);
    }

    int rc = zmq_bind(manager, "tcp://*:8888");
    if(rc != 0) {
        perror("Could not bind\n");
        zmq_close(manager);
        zmq_ctx_destroy(context);
        exit(1);
    }


    while(1){
        char recvbuffer[MAXLEN];
        char sendbuffer[MAXLEN];
        void *worker_array[MAXWORKERS] = {NULL, };

        strcpy(recvbuffer, s_recv(manager));
        printf("%s\n", recvbuffer);
        
        processRequest(recvbuffer, manager, context, worker_array);

   }
    return 0;
}

