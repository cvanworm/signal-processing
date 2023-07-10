#include<stdio.h>
#include<string.h>
#include<stdlib.h>
/**
 * Function provides the program with information about the memory
 * on a Linux Machine.  Use the proc files.  /proc/meminfo
 *
 * Return success, if < 0 then an error has occurred
 */
int getMemoryDetail(float *percentMemoryInUse, float *totalPhysicalMemory, float *freePhysicalMemory) {

    FILE *memoryinfo;
    char line[512];
    char tmp[512];
    char tmp1[512];
    double memTotal = 0;
    double memFree = 0;

    // Open /proc/cpuinfo file
    if ((memoryinfo = fopen("/proc/meminfo", "r")) == NULL) {return -1;}

    // Search until the "MemTotal" entry is found
    while(fgets(line, 512, memoryinfo)){
        sscanf(line, "%s", tmp);
        if((strcmp(tmp,"MemTotal:") == 0)){
            sscanf(line, "%*s %s", tmp);
            memTotal = strtod(tmp, NULL);
            break;
        }
    }

    // Search until the "MemFree" entry is found
    while(fgets(line, 512, memoryinfo)){
        sscanf(line, "%s", tmp);
        if((strcmp(tmp,"MemFree:") == 0)){
            sscanf(line, "%*s %s", tmp);
            memFree = strtod(tmp, NULL);
            break;
        }
    }

    fclose(memoryinfo);

    *totalPhysicalMemory = memTotal / 1024;
    *freePhysicalMemory = memFree / 1024;
    *percentMemoryInUse = (*totalPhysicalMemory - *freePhysicalMemory) / *totalPhysicalMemory;

    return 0;
}
