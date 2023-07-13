#include <unistd.h>
#include "sysdata.h"
#include "zmq_helpers.h"
#include "zmq_msgs.h"
#include "socket.h"

#define MAXLEN 512
char* systemDetails();

void *updateManager(void *input){
    printf("Thread created\n");
    while(1){
        sleep(1);
        // printf("Thread loop\n");
        char str[MAXLEN];
        char *sys = systemDetails();
        char host[55];
        gethostname(host, sizeof(host));
        host[MAXLEN - 1] = '\0';

        sprintf(str, "worker;checkin;%s;%s", host, sys);
        printf("%s",str);
        // s_send((void *)input,str);
        
    }
}

void checkForUpdate(){
    while(1){
        if(1){
            //remove worker from db
            return;
        }
        sleep(35);
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