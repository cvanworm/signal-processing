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
        sleep(5);
        // printf("Thread loop\n");
        char str[MAXLEN];
        //printf("1"\n);
        char *sys = systemDetails();
        //printf("2\n");
        char host[55];
        gethostname(host, sizeof(host));
        host[MAXLEN - 1] = '\0';
        //printf("3\n");

        sprintf(str, "worker;update;%s;%s", host, sys);
        printf("%s\n",str);
        s_send(input,str);
        
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