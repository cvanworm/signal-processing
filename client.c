#include <unistd.h>
#include "zmq_msgs.h"
#include "socket.h"
#include "file.h"
#include "cksum.h"

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
    printf("(client, manager) = (%s, %s)\n", host, server_addr);
    void *context=zmq_ctx_new();
    void* public = connect_to_supplicant(context, server_addr);
    //check if socket returned NULL
    if (public == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(public);
    	zmq_ctx_destroy(context);
    	
    }

    void *client = bind_socket(context, "tcp://*:8888");
        //Attemp to recieve file path from requester
    	char path[256];
        char sendbuffer[MAXLEN];
    	memset(path, 0, 256);
        strcpy(path, "test.csv");
    	// zmq_recv(client, path, 256,0);

        if (strlen(path) != 0){
		//single file transfer section
		//single file transfer will include a MD5 checksum value
			//printf("Supplicant chuck size: %d\n", FILE_CHUNK_SIZE);
			//calculate MD5 checksum for 
			char checksum_str[MD5_DIGEST_LENGTH * 2 + 1];
			if(calc_md5_sum(path, checksum_str)){
			printf("MD5 checksum for %s: %s\n",path, checksum_str);
            sprintf(sendbuffer, "client;checksum;%s;%s", "10.10.40.35", checksum_str);
			//send MD5 sum to requester before we start to send it the file
			zmq_send(public, sendbuffer, sizeof(sendbuffer), 0);
			}
			
			int result = send_file_to_requester(client, path);
			if (result != 0){return -1;}}
        //set path to 0s to avoid potential reruns
        memset(path, 0, 256);
    
    zmq_close(public);
    zmq_close(client);
    zmq_ctx_destroy(context);
    return 0;
}
