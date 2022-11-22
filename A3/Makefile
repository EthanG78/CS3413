CC = gcc

BASEFLAGS = -Wall -std=c99
DEBUG_FLAGS = -g

LDLIBS = -lm

OBJS = main.o util.o shell.o fat32.o

EXE = fat32

release: CFLAGS = $(BASEFLAGS)
release: $(EXE)

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h fat32.h
	$(CC) $(CFLAGS) -c util.c

shell.o: shell.c shell.h fat32.h
	$(CC) $(CFLAGS) -c shell.c

fat32.o: fat32.c fat32.h util.h
	$(CC) $(CFLAGS) -c fat32.c

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)

run:
	./$(EXE)
