CC=gcc
CFLAGS=-I.

all: client server clean

client : libmultmod
	$(CC) -c client.c -o client.o $(CFLAGS)
	$(CC) -o client client.o -L$(shell pwd) -lmultmod -lpthread $(CFLAGS)

server : libmultmod
	$(CC) -c server.c -o server.o $(CFLAGS)
	$(CC) -o server server.o -L$(shell pwd) -lmultmod -pthread $(CFLAGS)

libmultmod :
	$(CC) -o multmod.o -c multmod.c $(CFLAGS)
	ar rcs libmultmod.a multmod.o
	
clean :
	rm *.a *.o