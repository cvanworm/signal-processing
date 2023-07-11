#include "server.h"

int main(int argc, char** argv) {	
    
    //create ZMQ context
    void *context = zmq_ctx_new ();
    //Attemp to bind socket
    void *manager = bind_socket(context, "tcp://*:8888");

    while(1){
        char recvbuffer[MAXLEN];
        char sendbuffer[MAXLEN];
        void *worker_array[MAXWORKERS] = {NULL, };

        strcpy(recvbuffer, s_recv(manager));
        printf("Got: %s\n", recvbuffer);
        
        processRequest(recvbuffer, manager, context, worker_array);

   }
    return 0;
}

