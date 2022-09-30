CC=gcc
CFLAGS=-Wall -I.
DEPS=util.h sigHandlers.h
OBJS=util.o sigHandlers.o myshell.o
EXE=myshell

all: a1

a1: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(CFLAGS)

myshell.o: myshell.c $(DEPS)
	$(CC) -c myshell.c $(CFLAGS)

sigHandlers.o: sigHandlers.c $(DEPS)
	$(CC) -c sigHandlers.c $(CFLAGS)

util.o: util.c $(DEPS)
	$(CC) -c util.c $(CFLAGS)

run:
	./$(EXE)

debug:
	$(CC) -g -c util.c $(CFLAGS)
	$(CC) -g -c myshell.c $(CFLAGS)
	$(CC) -g -o $(EXE) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXE)