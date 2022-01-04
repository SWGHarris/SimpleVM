CC = gcc
CFLAGS = -g

all: lc3

lc3: main.o support.o
	$(CC) -o lc3 main.o support.o

main.o: main.c support.h
	$(CC) $(CFLAGS) -c main.c

support.o: support.c support.h
	$(CC) $(CFLAGS) -c support.c

clean:
	@rm -f *.o $(PROGS)
	@echo "now clean"
