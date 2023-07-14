#include "server.h"

int main(int argc, char** argv) {	
    
    //create ZMQ context
    void *context = zmq_ctx_new ();
    //Attemp to bind socket
    void *manager = bind_socket(context, "tcp://*:8888");

    struct workers worker_array[MAXWORKERS] = {NULL, };

    struct ThreadArgs args;
    
    args.socket = manager;
    args.context = context;
    args. w_arr = worker_array;

    while(1){
        char recvbuffer[MAXLEN];
        char sendbuffer[MAXLEN];
        pthread_t thread;

        strcpy(recvbuffer, s_recv(manager));
        printf("Got: %s\n", recvbuffer);
        
        args.buffer = recvbuffer;
        // Create a thread and pass the arguments
        pthread_create(&thread, NULL, processRequest, (void*)&args);
        //processRequest(recvbuffer, manager, context, worker_array);

   }
    return 0;
}

