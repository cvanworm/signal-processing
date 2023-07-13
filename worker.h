#include <unistd.h>
#include "sysdata.h"
#include "zmq_helpers.h"
#include "zmq_msgs.h"
#include "socket.h"

#define MAXLEN 512
char* systemDetails();

void *updateManager(void *socket){
    printf("Thread created\n");
    
    while(1){
        sleep(5);
        char str[MAXLEN];
        char *sys = systemDetails();
        char host[55];
        gethostname(host, sizeof(host));
        host[MAXLEN - 1] = '\0';

        sprintf(str, "worker;update;%s;%s", host, sys);

        s_send(socket,str);
    }

}

void *checkForUpdate(void *socket){
    while(1){
        char recvbuffer[MAXLEN];
        int rc = zmq_recv(socket, recvbuffer, MAXLEN, 0);
        if(rc == -1 && zmq_errno() == EAGAIN){
            printf("Timeout occured\n");
            //remove from database/worker_array
            return 0;
            
        }else{
            // char *command = "python3";
            // char *arguments[] = {"python3", "db/update.py", header[2], header[4], header[5], header[6], header[7], NULL};

            // int pid = fork();
            // if (pid == 0)
            // {
            //     execvp(command, arguments);
            // }
            // sleep(1);
            printf("Received update\n");
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