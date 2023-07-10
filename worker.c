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
    int port = 5555;
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_PAIR);
    
    int buffer_size = 1024 * 10;
	zmq_setsockopt(responder, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));

    int rc = zmq_bind(responder, "tcp://*:5555");
    if(rc != 0) {
        perror("Unable to bind to port\n");
        exit(1);
    }

    char send_buffer[MAXLEN], rec_buffer[MAXLEN];
   
    //Waits for a request from manager
    strcpy(rec_buffer, s_recv(responder));
    printf("Got: %s\n", rec_buffer);

    //Calculates system details to send to manager
    float upTime = 0;
    float loadAvg = 0;
    float memInUse;
    float totalMem;
    float freeMem;
    char str [255];
    char host[55];


    gethostname(host, sizeof(host));
    getMemoryDetail(&memInUse, &totalMem, &freeMem);
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);

    sprintf(str, "%s %li %f %f %f %f", host, numCores, freeMem, upTime, memInUse, loadAvg);

    printf("Sending sys data: %s\n", str);
    s_send(responder, str);
    
    //Closes socket and context
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}

/***
 * @param input: Input string (containing some sort of data)
 * @param output: Some transformed output (string)
 **/
void do_work(char *input, char *output) {
    int input_len = strlen(input);
    int i;
    // Just reversing it
    for(i = 0; i < input_len; i++) {
        output[i] = input[input_len - i - 1];
    }
    output[input_len] = '\0';
    return;
}
