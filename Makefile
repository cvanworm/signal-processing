CC = cc

all: cli srv wor testsrv testwor

testsrv: testsrv.c
		$(CC) -g -o testsrv testsrv.c -I./ -lzmq

testwor: testwor.c
		$(CC) -g -o testwor testwor.c -I./ -lzmq

cli: client.c
		$(CC) -g -o cli client.c -I./ -lzmq

srv: server.c
		$(CC) -g -o srv server.c -I./ -lzmq

wor: worker.c
		$(CC) -g -o wor worker.c -I./ -lzmq

clean:
		rm cli srv wor worker.db testsrv testwor

