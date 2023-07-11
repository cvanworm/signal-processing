CC = cc

all: cli srv wor tsrv tcli

tcli: testcli.c
		$(CC) -g -o tcli testcli.c -I./ -lzmq

tsrv: testsrv.c
		$(CC) -g -o tsrv testsrv.c -I./ -lzmq

cli: client.c
		$(CC) -g -o cli client.c -I./ -lzmq

srv: server.c
		$(CC) -g -o srv server.c -I./ -lzmq

wor: worker.c
		$(CC) -g -o wor worker.c -I./ -lzmq

clean:
		rm cli srv wor tsrv tcli
