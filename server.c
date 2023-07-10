// manager.c: Responds to a job and hands it off to another process/computer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include "zmq_helpers.h"
#include "zmq_msgs.h"

#define MAXLEN 512
#define MAXWORKERS 1

int populate_workers(
    int argc,
    char **argv,
    void **p_worker_array,
    int *p_n_workers,
    void *context
);

void close_workers(void **worker_array, int n_workers);

void print_help() {
    perror("USAGE: ./manager.out <list of workers' ip or name>\n");
    perror("Note: worker needs to be different from the manager!\n");
    return;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        print_help();
        exit(1);
    }

    // Manager is a responder to a client but is also a requester to its workers 
    void *context = zmq_ctx_new();
    if(context == NULL) {
        perror("Could not create zmq context\n");
        exit(1);
    }
    void *responder = zmq_socket(context, ZMQ_REP);
    if(responder == NULL) {
        perror("Could not create responder socket\n");
        exit(1);
    }

    int rc = zmq_bind(responder, "tcp://*:8888");
    if(rc != 0) {
        perror("Could not bind\n");
        zmq_close(responder);
        zmq_ctx_destroy(context);
        exit(1);
    }

    // Initialize connections to workers
    void *worker_array[MAXWORKERS] = {NULL, };
    int n_workers = 0;
    if(populate_workers(argc, argv, worker_array, &n_workers, context)) {
        zmq_close(responder);
        zmq_ctx_destroy(context);
        exit(1);
    }
    
    char cli_rec_buffer[MAXLEN];
    char wor_rec_buffer[MAXLEN];
    char send_buffer[MAXLEN]; 

    //Get request from client
    // strcpy(cli_rec_buffer, s_recv(responder));
    // printf("Got from client: %s\n", cli_rec_buffer);

    //Wait for workers to check in
    for(int i = 0; i<MAXWORKERS; i++){
        int numTokens;
        s_send(worker_array[i], "Send system data.");
        strcpy(wor_rec_buffer, s_recv(worker_array[i]));
        printf("Recieved: %s\n", wor_rec_buffer);

        char** params = splitStringOnSpaces(wor_rec_buffer, &numTokens);

        char *command = "python3";
        char *arguments[] = {"python3", "db/insert.py", params[0], params[1], params[2], params[3], params[4], params[5], NULL};
        
        int pid = fork();
        if ( pid == 0 ) {
		   execvp(command, arguments);
	    }
        sleep(1);

        free(params);
        
    }
    


    // // send it to a random worker
    // int worker_id = rand() % n_workers;
    // strcpy(send_buffer, rec_buffer);
    // s_send(worker_array[worker_id], send_buffer);

    // // wait on response from worker
    // strcpy(rec_buffer, s_recv(worker_array[worker_id]));
    // printf("Got from worker: %s\n", rec_buffer);

    // // send back to client
    // strcpy(send_buffer, rec_buffer);
    // s_send(responder, send_buffer);

    // Close worker connections and the manager (if infinitely polling this would likely not happen unless we handle a signal)
    close_workers(worker_array, n_workers);
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}

/***
 * @param argc: argc to main
 * @param argv: argv to main
 * @param worker_array: array of workers to be populated
 * @param p_n_workers: pointer to the number of workers; modified by this function
 * @param context: zmq context
 * @return int: 0 on success, nonzero on failure; prints error message to stderr
 **/
int populate_workers(
    int argc,
    char **argv,
    void **worker_array,
    int *p_n_workers,
    void *context
) {
    int n_workers = argc - 1; // assume everything but argv[0] is a worker
    if(n_workers > MAXWORKERS) {
        
        fprintf(stderr, "Had %d workers but the maximum is %d\n", n_workers, MAXWORKERS);
        return 1;
    }

    int i, worker_idx;
    for(i = 1, worker_idx = 0; i < argc; i++, worker_idx++) {
         // NOTE: A pub-sub/radio dish pattern would likely make more sense here
        void *worker = zmq_socket(context, ZMQ_PAIR);
        if(worker == NULL) {
            fprintf(stderr, "Failed on the %dth worker socket\n", worker_idx);
            close_workers(worker_array, worker_idx);
            return 1;
        }

        int buffer_size = 1024 * 10;
	    zmq_setsockopt(worker, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));
        int worker_port = 5555;
        char worker_addr[255];
        sprintf(worker_addr, "tcp://%s:%d", argv[i],worker_port);
        if(zmq_connect(worker, worker_addr)) {
            fprintf(stderr, "Failed to connect to the %dth address: %s\n", worker_idx, worker_addr);
            close_workers(worker_array, worker_idx + 1);
        }

        // connected, to assign worker to array
        worker_array[worker_idx] = worker;
    }

    *p_n_workers = n_workers;
    return 0;
}

/***
 * @param worker_array: Array of workers. Attempts to close all of them and sets them to NULL.
 * @param n_workers: Number of workers to be closed. Assumed to be less than/equal to the size worker_array
 * @return void
 **/
void close_workers(void **worker_array, int n_workers) {
    int i;
    for(i = 0; i < n_workers; i++) {
        if(worker_array[i] != NULL) {
            zmq_close(worker_array[i]);
            worker_array[i] = NULL;
        }
    }
}