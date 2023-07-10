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

    int port = 8888;
    char server_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[55 - 1] = '\0';
    sprintf(server_addr, "tcp://%s:%d", (argc < 2) ? "localhost" : argv[1], port);
    printf("(worker, server) = (%s, %s)\n", host, server_addr);
    // Pair pattern from worker to server/manager
    void *context = zmq_ctx_new();
    void *worker = zmq_socket(context, ZMQ_PAIR);

    int buffer_size = 1024 * 10;
	zmq_setsockopt(worker, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(worker, server_addr);

    char recvbuffer[MAXLEN];

    //while(1){
        //Calculates system details to send to manager
        float upTime = 0;
        float loadAvg = 0;
        float memInUse;
        float totalMem;
        float freeMem;
        char str [255];

        getMemoryDetail(&memInUse, &totalMem, &freeMem);
        long numCores = sysconf(_SC_NPROCESSORS_ONLN);

        sprintf(str, "worker;checkin;%s;%li;%f;%f;%f;%f", host, numCores, freeMem, upTime, memInUse, loadAvg);

        s_send(worker,str);

        strcpy(recvbuffer, s_recv(worker));
        printf("%s\n",recvbuffer);

        getMemoryDetail(&memInUse, &totalMem, &freeMem);
        numCores = sysconf(_SC_NPROCESSORS_ONLN);

        sprintf(str, "worker;update;%s;%li;%f;%f;%f;%f", host, numCores, freeMem, upTime, memInUse, loadAvg);
        s_send(worker,str);

        strcpy(recvbuffer, s_recv(worker));
        printf("%s\n",recvbuffer);

    //}
    
    
    // while(1){
    //     s_send(worker, "Worker");
    //     strcpy(recvbuffer, s_recv(worker));
    //     printf("%s\n", recvbuffer);
    //     s_send(worker, "Worker follow up");
    //     strcpy(recvbuffer, s_recv(worker));
    //     printf("%s\n", recvbuffer);

    // }
    
    
    //Closes socket and context
    zmq_close(worker);
    zmq_ctx_destroy(context);
    return 0;
}
