#include<string.h>
#include<zmq.h>


static char * s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    if (size > 255)
        size = 255;
    buffer [size] = 0;
    /* use strndup(buffer, sizeof(buffer)-1) in *nix */
    return strdup (buffer);
}

static void s_send (void *socket, const char *string) {
	zmq_send (socket, strdup(string), strlen(string), 0);
}
