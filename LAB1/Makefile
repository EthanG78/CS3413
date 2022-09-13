CC=gcc
CFLAGS=-Wall -I.
DEPS=hellomake.h
OBJS=hellomake.o hellofunc.o
EXE=hellomake

hellomake: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(CFLAGS)

hellomake.o: hellomake.c $(DEPS)
	$(CC) -c hellomake.c $(CFLAGS)

hellofunc.o: hellofunc.c $(DEPS)
	$(CC) -c hellofunc.c $(CFLAGS)

run:
	./$(EXE)

debug:
	$(CC) -g -c hellomake.c $(CFLAGS)
	$(CC) -g -c hellofunc.c $(CFLAGS)
	$(CC) -g -o $(EXE) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXE)