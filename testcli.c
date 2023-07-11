#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <server.h>

int main(int argc, char** argv) {
    int port = 8888;
    char server_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';
    sprintf(server_addr, "tcp://%s:%d", (argc < 2) ? "localhost" : argv[1], port);
    printf("(client, server) = (%s, %s)\n", host, server_addr);
    // Requester-Responder pattern from client to server/manager
    void *context = zmq_ctx_new();
    void *client = zmq_socket(context, ZMQ_REQ);
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(client, server_addr);
    
    char rec[MAXLEN];

    s_send(client, "Message");

    strcpy(rec, s_recv(client));
    printf("%s\n", rec);

    return 0;

}