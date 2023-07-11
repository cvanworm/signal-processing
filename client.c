// client.c: Requests for some job to be completed
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include "zmq_msgs.h"
#include "socket.h"

#define MAXLEN 512

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("USAGE: ./client.out <server name>\n");
        printf("Because no server is present, using localhost\n");
    }
    
    int port = 8888;
    char server_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';
    sprintf(server_addr, "tcp://%s:%d", argv[1], port);
    printf("(client, server) = (%s, %s)\n", host, server_addr);
    void *context=zmq_ctx_new();
    void* client = connect_socket(context, server_addr);
    //check if socket returned NULL
    if (client == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(client);
    	zmq_ctx_destroy(context);
    	
    }

    // set up message/data to be sent
    char sendbuffer[MAXLEN], recvbuffer[MAXLEN];


    // send message to server
    s_send(client, "client;dowork");
    
    // strcpy(recvbuffer, s_recv(client));
    // printf("%s",recvbuffer);

    zmq_close(client);
    zmq_ctx_destroy(context);
    return 0;
}
