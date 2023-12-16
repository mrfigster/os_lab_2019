CC=gcc
CFLAGS=-I.

all: process_memory parallel_sum parallel_min_max clean

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

process_memory : 
	$(CC) -o process_memory process_memory.c

parallel_sum : utils.o lib_sum.o
	$(CC) -pthread -o parallel_sum utils.o lib_sum.o parallel_sum.c $(CFLAGS)

clean :
	rm *.o