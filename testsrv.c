#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <server.h>

int main(int argc, char** argv) {
    // Manager is a responder to a client but is also a requester to its workers 
    void *context = zmq_ctx_new();
    if(context == NULL) {
        perror("Could not create zmq context\n");
        exit(1);
    }
    void *responder = zmq_socket(context, ZMQ_REP);
    if(responder == NULL) {
        perror("Could not create responder socket\n");
        exit(1);
    }

    int rc = zmq_bind(responder, "tcp://*:8888");
    if(rc != 0) {
        perror("Could not bind\n");
        zmq_close(responder);
        zmq_ctx_destroy(context);
        exit(1);
    }

    char rec[MAXLEN];

    strcpy(rec, s_recv(responder));
    printf("%s\n",rec);

    s_send(responder,"Got");

    return 0;
}