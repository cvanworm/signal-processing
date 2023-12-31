#include "worker.h"

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
    printf("(worker, manager) = (%s, %s)\n", host, server_addr);
    void *context=zmq_ctx_new();
    void* public = connect_to_supplicant(context, server_addr);
    //check if socket returned NULL
    if (public == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(public);
    	zmq_ctx_destroy(context);
    	
    }

        char str [MAXLEN];
        char recvbuffer[MAXLEN];

        //Calculates system details to send to public
        char *sys = systemDetails();

        sprintf(str, "worker;checkin;%s;%s;%s", "10.10.40.35",host, sys);

        free(sys);

        s_send(public,str);
        
        strcpy(recvbuffer, s_recv(public));
        printf("%s\n",recvbuffer);

        zmq_close(public);

    //Attempt to bind to private sockets
    void *worker = bind_socket(context, "tcp://*:8888");

    void *hb = bind_socket(context, "tcp://*:5555");

    while(1){
       
        strcpy(recvbuffer, s_recv(worker));
        
        if(strcmp(recvbuffer, "Worker populated")==0){
            printf("%s\n",recvbuffer);
            //Create thread that sends heartbeat to manager every 30 seconds
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, updateManager, hb);
            int ret = pthread_detach(thread_id);
            if(ret != 0){
                printf("Error occured with thread.");
                exit(0);
            }
        }else if(strcmp(recvbuffer, "work")==0){

            printf("work\n");
            s_send(worker, "Finished Work");

        }
        
    }
   

    //Closes socket and context
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return 0;
}
