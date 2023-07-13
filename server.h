#include "worker.h"
#include <pthread.h>

#define MAXWORKERS 10

struct workers {
    void *sock;
    char *host;
};

int getNumberElements(struct workers* array) {
    int count = 0;
    while (array[count].sock != NULL) {
        count++;
    }
    return count;
}

int populate_workers(
    struct workers* worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip
);

void close_workers(void **worker_array, int n_workers);

void processRequest(char *request, void *socket, void *context, struct workers* worker_array)
{
    char sendbuffer[MAXLEN];
    char recvbuffer[MAXLEN];
    int numTokens;
    int n_workers;
    int idx;

    char **header = splitStringOnSemiColons(request, &numTokens);
    
    // List of worker actions
    if (strcmp(header[0], "worker") == 0)
    {
        // Adds worker info to the database for later use
        if (strcmp(header[1], "checkin") == 0)
        {

            s_send(socket, "Checkin Recieved");
            
            printf("Checkin recieved: populating worker\n");
            if(populate_workers(worker_array, &n_workers, &idx, context, header[2])) {
                exit(1);
            }
             // char *command = "python3";
            // char *arguments[] = {"python3", "db/insert.py", header[3], header[4], header[5], header[6], header[7], header[8], NULL};

            // int pid = fork();
            // if (pid == 0)
            // {
            //     execvp(command, arguments);
            // }
            //sleep(1);
            s_send(worker_array[idx].sock, "Worker populated");

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, checkForUpdate, worker_array[idx].sock);
            int ret = pthread_detach(thread_id);
            if(ret != 0){
                printf("Error occured with thread.");
                exit(0);
            }
            
        }
        
    }
    else if (strcmp(header[0], "client") == 0)
    {
        if (strcmp(header[1], "file") == 0)
        {
            // This will also have to be connected with the requesting client somehow?
            s_send(worker_array[0].sock, "work");
            
            strcpy(recvbuffer, s_recv(worker_array[0].sock));
            s_send(socket, recvbuffer);
        }
    }
    free(header);
}

void close_workers(void **worker_array, int n_workers) {
    int i;
    for(i = 0; i < n_workers; i++) {
        if(worker_array[i] != NULL) {
            zmq_close(worker_array[i]);
            worker_array[i] = NULL;
        }
    }
}

int populate_workers(
    struct workers* worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip
) {
    //printf("IP: %s\n", worker_ip);
    int n_workers = getNumberElements(worker_array);
    *worker_idx = n_workers;
    if(n_workers > MAXWORKERS) {
        
        fprintf(stderr, "Had %d workers but the maximum is %d\n", n_workers, MAXWORKERS);
        return 1;
    }

    int port = 8888;
    char worker_addr[MAXLEN];
    char host[55];
    gethostname(host, sizeof(host));
    host[MAXLEN - 1] = '\0';
    sprintf(worker_addr, "tcp://%s:%d", worker_ip, port);
    // printf("(manager, worker) = (%s, %s)\n", host, worker_addr);
    void* worker = connect_socket(context, worker_addr);

    int timeout = 6000; // 5 sec
    int opt = zmq_setsockopt(worker, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    if(opt == -1){
        printf("Error setting opt");
    }
    
    //check if socket returned NULL
    if (worker == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(worker);
    	zmq_ctx_destroy(context);
    }

    struct workers w;
    w.sock = worker;
    w.host = host;


    // connected, to assign worker to array
    worker_array[*worker_idx] = w;

    *p_n_workers = n_workers;
    return 0;
}