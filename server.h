#include "worker.h"
#include <pthread.h>

#define MAXWORKERS 10

pthread_mutex_t mutex; // Declare a mutex variable


// int getNumberElements(struct workers* array) {
//     int count = 0;
//     while (array[count].sock != NULL) {
//         count++;
//     }
//     return count;
// }

int populate_workers(
    struct workers* worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip
);

//void processRequest(char *request, void *socket, void *context, struct workers* worker_array)
void* processRequest(void* args)
{
    struct ThreadArgs* myArgs = (struct ThreadArgs*)args;
    char *request = myArgs->buffer;
    void *socket = myArgs->socket;
    void *context = myArgs->context;
    struct workers* worker_array = myArgs->w_arr;

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

            //Creates thread to check for worker heartbeat
            struct ThreadArgs args;
            args.w_arr = worker_array;
            args.socket = worker_array[idx].sock;
            args.host = worker_array[idx].host;

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, checkForUpdate, (void*)&args);
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
            pthread_mutex_lock(&mutex);
            s_send(worker_array[0].sock, "work");
            //update database and show work is in progress
            
            strcpy(recvbuffer, s_recv(worker_array[0].sock));
            printf("%s\n", recvbuffer);
            pthread_mutex_unlock(&mutex);
            s_send(socket, recvbuffer);
        }
    }
    free(header);
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