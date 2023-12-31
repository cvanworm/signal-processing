#include <unistd.h>
#include <pthread.h>
#include "sysdata.h"
#include "zmq_helpers.h"
#include "zmq_msgs.h"
#include "socket.h"

#define MAXLEN 512
// pthread_mutex_t mutex; // Declare a mutex variable
// int shouldPause = 0;

struct ThreadArgs {
    char* buffer;
    void* socket;
    void* context;
    char* host;
    struct workers* w_arr;
};

char* systemDetails();
void close_worker(struct workers* worker_array, char *host);

//Grabs system details to sent to update manager: HEARTBEAT
void *updateManager(void *socket){
    //printf("Thread created\n");
    while(1){

        // Send heartbeat every 30 sec
        sleep(30);
        char str[MAXLEN];
        char *sys = systemDetails();
        char host[55];
        gethostname(host, sizeof(host));
        host[MAXLEN - 1] = '\0';

        sprintf(str, "worker;update;%s;%s", host, sys);

        s_send(socket,str);
    }

}

//Updates DB if heartbeat is received
//Removes from DB if none is found
void *checkForUpdate(void* args){
    struct ThreadArgs* myArgs = (struct ThreadArgs*)args;
    char *host = myArgs->host;
    void *socket = myArgs->socket;
    struct workers* worker_array = myArgs->w_arr;


    while(1){

        // Check for updates from worker
        char recvbuffer[MAXLEN];
        int rc = zmq_recv(socket, recvbuffer, MAXLEN, 0);

        // If no update in 40 sec, remove worker
        if(rc == -1 && zmq_errno() == EAGAIN){
            printf("Timeout occured on %s\n", host);
            //remove from database/worker_array
            close_worker(worker_array, host);
            //printf("Closed worker: %s\n", host);
            return 0;
            
        }

        int numTokens;
        char **header = splitStringOnSemiColons(recvbuffer, &numTokens);
        
        // If update comes through, update DB
        if(strcmp(header[0], "worker")==0){
            // char *command = "python3";
            // char *arguments[] = {"python3", "db/update.py", header[2], header[4], header[5], header[6], header[7], NULL};

            // int pid = fork();
            // if (pid == 0)
            // {
            //     execvp(command, arguments);
            // }
            // sleep(1);
            printf("Received update from %s\n", host);
        }else{
            printf("Error, not an update\n");
            return 0;
        }
    }     
}

char* systemDetails(){
    float upTime = 0;
    float loadAvg = 0;
    float memInUse;
    float totalMem;
    float freeMem;
    char* str = malloc(255 * sizeof(char));  

    getMemoryDetail(&memInUse, &totalMem, &freeMem);
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);

    sprintf(str, "%li;%f;%f;%f;%f",numCores, freeMem, upTime, memInUse, loadAvg);

    return str;
}

void close_worker(struct workers* worker_array, char *host) {
    int n_workers = getNumberElements(worker_array);
    int i;
    for(i = 0; i < n_workers; i++) {
        if(strcmp(worker_array[i].host, host)==0) {
            // struct workers temp = worker_array[i];
            zmq_close(worker_array[i].work);
            zmq_close(worker_array[i].hb);
            if(n_workers > 1){
                //printf("Replacing with: %s\n", worker_array[n_workers-1].host);
                worker_array[i] = worker_array[n_workers-1];
                //printf("New host: %s\n", worker_array[i].host);
            }else{
                worker_array[n_workers-1].work = NULL;
                worker_array[n_workers-1].hb = NULL;
                worker_array[n_workers-1].host = NULL;
            }

            //printf("Final host: %s\n", worker_array[i].host);
            
        }
    }
}