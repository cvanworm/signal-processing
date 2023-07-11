#include<string.h>
#include<zmq.h>

#define MAXLEN 512
static char * s_recv (void *socket) {
    char buffer [MAXLEN];
    int size = zmq_recv (socket, buffer, MAXLEN-1, 0);
    if (size == -1)
        return NULL;
    if (size > MAXLEN-1)
        size = MAXLEN-1;
    buffer [size] = 0;
    /* use strndup(buffer, sizeof(buffer)-1) in *nix */
    return strdup (buffer);
}

static void s_send (void *socket, const char *string) {
	zmq_send (socket, strdup(string), strlen(string), 0);
}
