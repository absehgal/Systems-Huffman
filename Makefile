CC = gcc
CFLAGS = -Wall -ansi -pedantic

all: hencode.o hdecode.o structure.o structure.h
	$(CC) -o hencode structure.o hencode.c 
	$(CC) -o hdecode structure.o hdecode.c 

hdecode.o: hdecode.c structure.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c

hencode.o: hencode.c structure.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c

structure.o: structure.c structure.h
	gcc -c structure.c

clean:
	rm *.o
