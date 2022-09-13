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

debug: $(OBJS)
	$(CC) -g -o $(EXE) $(OBJS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(EXE)