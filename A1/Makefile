CC=gcc
CFLAGS=-Wall -I.
DEPS=util.h sigHandlers.h builtins.h jobs.h
OBJS=util.o sigHandlers.o builtins.o jobs.o myshell.o
EXE=myshell

all: a1

a1: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(CFLAGS)

myshell.o: myshell.c $(DEPS)
	$(CC) -c myshell.c $(CFLAGS)

jobs.o: jobs.c $(DEPS)
	$(CC) -c jobs.c $(CFLAGS)

builtins.o: builtins.c $(DEPS)
	$(CC) -c builtins.c $(CFLAGS)

sigHandlers.o: sigHandlers.c $(DEPS)
	$(CC) -c sigHandlers.c $(CFLAGS)

util.o: util.c $(DEPS)
	$(CC) -c util.c $(CFLAGS)

run:
	./$(EXE)

debug:
	$(CC) -g -c util.c $(CFLAGS)
	$(CC) -g -c sigHandlers.c $(CFLAGS)
	$(CC) -g -c builtins.c $(CFLAGS)
	$(CC) -g -c jobs.c $(CFLAGS)
	$(CC) -g -c myshell.c $(CFLAGS)
	$(CC) -g -o $(EXE) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXE)