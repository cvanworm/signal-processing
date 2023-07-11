#include "worker.h"

void processRequest(char *request, void *socket, void *context, void **worker_array)
{
    char sendbuffer[MAXLEN];
    char recvbuffer[MAXLEN];
    int numTokens;
    int n_workers, idx = 0;

    char **header = splitStringOnSemiColons(request, &numTokens);
    
    // List of worker actions
    if (strcmp(header[0], "worker") == 0)
    {
        // Adds worker info to the database for later use
        if (strcmp(header[1], "checkin") == 0)
        {
            // char *command = "python3";
            // char *arguments[] = {"python3", "db/insert.py", header[3], header[4], header[5], header[6], header[7], header[8], NULL};

            // int pid = fork();
            // if (pid == 0)
            // {
            //     execvp(command, arguments);
            // }
            // sleep(1);

            s_send(socket, "Checkin Recieved");

            // if(populate_workers(worker_array, &n_workers, &idx, context, header[2])) {
            //     exit(1);
            // }
            // printf("Worker populated\n");

            
        }
        else if (strcmp(header[1], "waiting") == 0)
        {
            // Not sure what to do here. Connection is unclear currently.
            // Want to wait until some data is available to send to worker, but not sure how to
            // implement this yet.
            s_send(socket, header[1]);

            // Receives some response that is the work done on the data
        }
        else if (strcmp(header[1], "ans") == 0)
        {
            // This will also have to be connected with the requesting client somehow?
            s_send(socket, "Received answer");
            
        }
        // After doing some work, the worker will send an update on its system details
        else if (strcmp(header[1], "update") == 0)
        {
            char *command = "python3";
            char *arguments[] = {"python3", "db/update.py", header[2], header[4], header[5], header[6], header[7], NULL};

            int pid = fork();
            if (pid == 0)
            {
                execvp(command, arguments);
            }
            sleep(1);

            sprintf(sendbuffer, "Update recieved");
            s_send(socket, sendbuffer);
        }
    }
    else if (strcmp(header[0], "client") == 0)
    {
        if (strcmp(header[1], "dowork") == 0)
        {
            // This will also have to be connected with the requesting client somehow?
            int worker_id = 0;
            
            s_send(worker_array[worker_id], "wOrK");
        }
    }
    free(header);
}
