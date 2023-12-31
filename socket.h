#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

void* connect_to_supplicant(void *context, const char *server_address){
	//create socket and attempt to connect to provided context
	void *socket = zmq_socket(context, ZMQ_PAIR);
	if (socket == NULL){
		printf("CTS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
	}	
	
	//attempt to connect socket to provided server
	int rc = zmq_connect(socket, server_address);
	if (rc != 0){
		printf("CTS_error 2: Failed to Connect to server. Error: %s\n", zmq_strerror(zmq_errno()));
		return NULL;
		zmq_close(socket);
	}
	//printf("Connection succesful to server at %s\n", server_address);
	return socket;}

	void* bind_socket(void *context, const char *server_address){
		void *socket = zmq_socket(context, ZMQ_PAIR);
		//check if socket was successfully created
		if (socket == NULL){
			printf("BS_error 1: Failed to create socket. ERROR: %s\n", zmq_strerror(zmq_errno()));
			return NULL;}
		
		//attempt to bind socket
		int rc = zmq_bind(socket, server_address);
		//check status of bind operation
		if (rc != 0){
			printf("BS_error 2: Failed to bind socket. Error: %s\n", zmq_strerror(zmq_errno()));
			return NULL;
			zmq_close(socket);}
	
	//Return pointer to socket
	return socket;}