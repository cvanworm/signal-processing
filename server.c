#include "server.h"

int main(int argc, char** argv) {	
    
    //create ZMQ context
    void *context = zmq_ctx_new ();
    //Attemp to bind socket
    void *manager = bind_socket(context, "tcp://*:8888");

    void *worker_array[MAXWORKERS] = {NULL, };

    while(1){
        char recvbuffer[MAXLEN];
        char sendbuffer[MAXLEN];

        strcpy(recvbuffer, s_recv(manager));
        printf("Got: %s\n", recvbuffer);
        
        processRequest(recvbuffer, manager, context, worker_array);

        // s_send(worker_array[0],"test");

   }
    return 0;
}

