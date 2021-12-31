CC = gcc

all: lc3

lc3: main.o support.o header.o
	$(CC) -o lc3 main.o support.o header.o

main.o: main.c support.h
	$(CC) -c main.c

support.o: support.c support.h
	$(CC) -c support.c

clean:
	@rm -f *.o $(PROGS)
	@echo "now clean"
