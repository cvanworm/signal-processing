#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#define FILE_CHUNK_SIZE 1024

// Binds socket as a ZMQ_PAIR type
void* bind_socket(void *context, const char *address){
	void *socket = zmq_socket(context, ZMQ_PAIR);
	if (socket == NULL){
		printf("BS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;}
	
	int buffer_size = FILE_CHUNK_SIZE * 10;
	zmq_setsockopt(socket, ZMQ_RCVBUF, &buffer_size, sizeof(buffer_size));	
	
	//attempt to bind socket
	int rc = zmq_bind(socket, address);
	if (rc != 0){
		printf("BS_error 2: Failed to bind socket. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);}
	//printf("Connection succesful to client");
	return socket;
}

void* connect_socket(void *context, const char *address){
	//create socket and attempt to connect to provided context
	void *socket = zmq_socket(context, ZMQ_PAIR);
	if (socket == NULL){
		printf("CTS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
	}	
	
	int buffer_size = FILE_CHUNK_SIZE * 10;
	zmq_setsockopt(socket, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));

    int timeout = 6000; // 5 sec
    int opt = zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    if(opt == -1){
        printf("Error setting opt");
    }
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(socket, address);
	if (rc != 0){
		printf("CTS_error 2: Failed to Connect to server. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);
	}

	//printf("Connection succesful to server at %s\n", address);
	return socket;
	
}