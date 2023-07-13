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
    int i = 0;
    while(1){
        char recvbuffer[MAXLEN];
        int rc = zmq_recv(socket, recvbuffer, MAXLEN, 0);
        if(rc == -1 && zmq_errno() == EAGAIN){
            printf("Timeout occured\n");
            return 0;
            //remove from database/worker_array
        }else{
            printf("Received update\n");
            if(i%2 == 0){
                printf("work\n");
                s_send(socket, "work");
            }
            i++;
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