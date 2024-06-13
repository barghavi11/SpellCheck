CC = gcc
CFLAGS = -Wall -fsanitize=address -std=c99 -O2

all: spchk 

spchk.o: spchk.c arraylist.h
	$(CC) $(CFLAGS) -c spchk.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CFLAGS) -c arraylist.c

spchk: spchk.o arraylist.o
	$(CC) $(CFLAGS) spchk.o arraylist.o -o spchk


clean:
	rm *.o  