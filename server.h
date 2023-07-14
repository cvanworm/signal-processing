#include "worker.h"

#define MAXWORKERS 10

int findWorker(char *host, struct workers* arr);
int populate_workers(
    struct workers* worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip,
    char *host
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
            if(populate_workers(worker_array, &n_workers, &idx, context, header[2], header[3])) {
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

            int index = findWorker(header[3], worker_array);
            if(index == -1){
                printf("Error finding worker");
                exit(0);
            }
            s_send(worker_array[index].work, "Worker populated");

            //Creates thread to check for worker heartbeat
            struct ThreadArgs args;
            args.w_arr = worker_array;
            args.socket = worker_array[index].hb;
            args.host = worker_array[index].host;

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
            s_send(worker_array[0].work, "work");
            //update database and show work is in progress
            
            strcpy(recvbuffer, s_recv(worker_array[0].work));
            printf("%s\n", recvbuffer);
            s_send(socket, recvbuffer);

        }
    }
    free(header);
}

int findWorker(char *host, struct workers* arr){
    int n_workers = getNumberElements(arr);
    int i;
    for(i=0; i<n_workers;i++){
        if(strcmp(arr[i].host, host)==0){
            return i;
        }
    }
    return -1;
}

int populate_workers(
    struct workers* worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip,
    char *host
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
    // char host[55];
    // gethostname(host, sizeof(host));
    // host[MAXLEN - 1] = '\0';
    sprintf(worker_addr, "tcp://%s:%d", worker_ip, port);
    // printf("(manager, worker) = (%s, %s)\n", host, worker_addr);
    void* worker = connect_socket(context, worker_addr);

    //check if socket returned NULL
    if (worker == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(worker);
    }

    port = 5555;
    sprintf(worker_addr, "tcp://%s:%d", worker_ip, port);
    void* hb = connect_socket(context, worker_addr);

    int timeout = 40000; // 40 sec
    int opt = zmq_setsockopt(hb, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    if(opt == -1){
        printf("Error setting opt");
    }

    //check if socket returned NULL
    if (hb == NULL){
    
    	printf("ERROR 1 Failed to connect to server.");
    	zmq_close(hb);
    }
    

    struct workers w;
    w.work = worker;
    w.hb = hb;
    w.host = host;


    // connected, to assign worker to array
    worker_array[*worker_idx] = w;

    *p_n_workers = n_workers;
    return 0;
}