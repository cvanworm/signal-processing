#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <zmq.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define FILE_CHUNK_SIZE 1048576 //1MB
#define BUFFER_SIZE 256

//Send file over zmq to requester
int send_file_to_requester(void *socket, char *path){
	
	//attempt to open file
	FILE* file = fopen(path, "rb");
	
	//send status back to requester regarding if the file existed or could be opened
	if (file == NULL){
		char error_message[BUFFER_SIZE];
		snprintf(error_message, sizeof(error_message), "Error opening file: %s", strerror(errno));
		zmq_send(socket, error_message, strlen(error_message), 0);
		return -1;
	}
	//if the file exists and is able to be opened then send back the string "continue"
	//there is probably a more efficent way to do this but I was just trying to handle errors here
	else{
		char response[]="Continue";
		zmq_send(socket, response, strlen(response), 0);
	
	}
	
	//////////////////////////////
	//Begin actual file transfer//
	//////////////////////////////
	
	//move the file position indicator to the end of the file specified by the FILE pointer file
	fseek(file, 0, SEEK_END);
	//determine the current position of the file position indicator
	long file_size = ftell(file);
	//reset file position indicatior to the beginning of the file 
	rewind(file);
	
	//send the file size to the receiver
	zmq_send(socket, &file_size, sizeof(long), 0);
	
	//send the file content in chunks
	char buffer[FILE_CHUNK_SIZE];
	size_t remaining_bytes = file_size;
	while (remaining_bytes > 0) {
		size_t chunk_size = fread(buffer, sizeof(char), FILE_CHUNK_SIZE, file);
		zmq_send(socket, buffer, chunk_size, 0);
		remaining_bytes -= chunk_size;
	}
	//Cleanup
	fclose(file);
	
	return 0;}

// Recieves file over zmq from sender
int receive_file_from_device(void *socket,const char *local_path){
	
	//Check for repsonse from supplicant on status of file
	//I.E. wheter it can be opened or not, or if it even exists
	char response[256];
	memset(response, 0, 256);
	zmq_recv(socket, response, 255, 0);
	if(strstr(response, "Error") != NULL){
	printf("%s\n", response);
	return -1;}
	//If the file does exist and is able to be opened the server will send back "continue"
	else if(strcmp(response, "Continue") == 0){}
	
	//receive the file size from the client
	long file_size;
	zmq_recv(socket, &file_size, sizeof(long), 0);
	//recieve the file content in chunks
	char buffer[FILE_CHUNK_SIZE];
	size_t remaining_bytes = file_size;
	FILE* file = fopen(local_path, "wb");
	while (remaining_bytes > 0){
		size_t chunk_size = zmq_recv(socket, buffer, FILE_CHUNK_SIZE, 0);
		fwrite(buffer, sizeof(char), chunk_size, file);
		remaining_bytes -= chunk_size;}
	
	//cleanup
	fclose(file);
                     
	return 0;}

    #endif