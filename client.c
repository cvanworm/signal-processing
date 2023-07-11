// client.c: Requests for some job to be completed
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include "zmq_msgs.h"

#define MAXLEN 512

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("USAGE: ./client.out <server name>\n");
        printf("Because no server is present, using localhost\n");
    }

    int port = 5555;
    char server_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';
    sprintf(server_addr, "tcp://%s:%d", (argc < 2) ? "localhost" : argv[1], port);
    printf("(client, server) = (%s, %s)\n", host, server_addr);
    // Requester-Responder pattern from client to server/manager
    void *context = zmq_ctx_new();
    void *client = zmq_socket(context, ZMQ_PAIR);

    int buffer_size = 1024 * 10;
	zmq_setsockopt(client, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(client, server_addr);
    // set up message/data to be sent
    char sendbuffer[MAXLEN], recvbuffer[MAXLEN];
    // strcpy(sendbuffer, host);
    //while(1){
        // send message to server
        s_send(client, "client;dowork");

        // block and wait for response from server
        // strcpy(recvbuffer, s_recv(client)); 
        // printf("%s\n", recvbuffer);
    //     s_send(client, "Client follow up");

    //     // block and wait for response from server
    //     strcpy(recvbuffer, s_recv(client)); 
    //     printf("%s\n", recvbuffer);
    //}
    

    zmq_close(client);
    zmq_ctx_destroy(context);
    return 0;
}
