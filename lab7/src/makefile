CC=gcc
CFLAGS=-I.

all: tcp udp clean

tcp: tcpclient tcpserver

tcpclient : 
	$(CC) -c tcpclient.c -o tcpclient.o $(CFLAGS)
	$(CC) -o tcpclient tcpclient.o -L$(shell pwd) $(CFLAGS)

tcpserver : 
	$(CC) -c tcpserver.c -o tcpserver.o $(CFLAGS)
	$(CC) -o tcpserver tcpserver.o -L$(shell pwd) $(CFLAGS)

udp: udpclient udpserver

udpclient : 
	$(CC) -c udpclient.c -o udpclient.o $(CFLAGS)
	$(CC) -o udpclient udpclient.o -L$(shell pwd) $(CFLAGS)

udpserver : 
	$(CC) -c udpserver.c -o udpserver.o $(CFLAGS)
	$(CC) -o udpserver udpserver.o -L$(shell pwd) $(CFLAGS)
	
clean :
	rm *.o