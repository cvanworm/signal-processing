CC = cc

all: cli srv wor

cli: client.c
		$(CC) -g -o cli client.c -I./ -lzmq -lpthread -lcrypto

srv: server.c
		$(CC) -g -o srv server.c -I./ -lzmq -lpthread -lcrypto

wor: worker.c
		$(CC) -g -o wor worker.c -I./ -lzmq -lpthread -lcrypto

clean:
		rm cli srv wor
