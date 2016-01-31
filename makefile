// Tested and working on Linux

.c.o:
	gcc -g -c $?

# compile client and server
all: client server

# compile client only
client: SimpClient.c  util.c
	gcc -g -o SimpClient SimpClient.c

#compile server only
server: SimpServer.c util.c
	gcc -g -o SimpServer SimpServer.c
