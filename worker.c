#include <unistd.h>
#include "sysdata.h"
#include "zmq_msgs.h"
#include "socket.h"

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
    printf("(worker, manager) = (%s, %s)\n", host, server_addr);
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

        sprintf(str, "worker;checkin;%s;%s;%li;%f;%f;%f;%f", "10.10.40.35",host, numCores, freeMem, upTime, memInUse, loadAvg);

        s_send(public,str);

        strcpy(recvbuffer, s_recv(public));
        printf("%s\n",recvbuffer);

        zmq_close(public);

    //Attempt to bind to private socket
    void *worker = bind_socket(context, "tcp://*:5555");
    while(1){
        strcpy(recvbuffer, s_recv(worker));
        printf("%s\n",recvbuffer);
    }
   

    //Closes socket and context
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return 0;
}
