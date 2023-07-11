#include "zmq_helpers.h"
#include "zmq_msgs.h"

#define MAXLEN 512
#define MAXWORKERS 10

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
    void **worker_array,
    int *p_n_workers,
    int *worker_idx,
    void *context,
    char *worker_ip
) {
    printf("IP: %s\n", worker_ip);
    // char *fbuffer;
    // long numbytes;
    // int numTokens;

    // fbuffer = openFile(fname, &numbytes);
    // fbuffer = formatString(fbuffer, numbytes);

    // char **ip_arr = splitStringOnSemiColons(fbuffer, &numTokens);
    int n_workers = getNumberElements(worker_array);
    *worker_idx = n_workers;
    if(n_workers > MAXWORKERS) {
        
        fprintf(stderr, "Had %d workers but the maximum is %d\n", n_workers, MAXWORKERS);
        return 1;
    }

    // int i, worker_idx;
    // for(i = 0, worker_idx = 0; i < n_workers; i++, worker_idx++) {
        void *worker = zmq_socket(context, ZMQ_REP);
        if(worker == NULL) {
            fprintf(stderr, "Failed on the %dth worker socket\n", *worker_idx);
            close_workers(worker_array, *worker_idx);
            return 1;
        }

        // int buffer_size = 1024 * 10;
	    // zmq_setsockopt(worker, ZMQ_SNDBUF, &buffer_size, sizeof(buffer_size));
        int worker_port = 8888;
        char worker_addr[255];
        sprintf(worker_addr, "tcp://%s:%d", worker_ip,worker_port);
        if(zmq_connect(worker, worker_addr)) {
            fprintf(stderr, "Failed to connect to the %dth address: %s\n", *worker_idx, worker_addr);
            close_workers(worker_array, *worker_idx + 1);
        }

        // connected, to assign worker to array
        worker_array[*worker_idx] = worker;
    //}

    *p_n_workers = n_workers;
    return 0;
}