CC = cc

all: cli srv wor

cli: client.c
		$(CC) -g -o cli client.c -I./ -lzmq

srv: server.c
		$(CC) -g -o srv server.c -I./ -lzmq

wor: worker.c
		$(CC) -g -o wor worker.c -I./ -lzmq

clean:
		rm cli srv wor
