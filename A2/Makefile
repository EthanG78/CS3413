CC = gcc

BASEFLAGS = -Wall -pthread -std=c99
NODEBUG_FLAGS = -dN DEBUG 
DEBUG_FLAGS = -g

LDLIBS = -lcurses -pthread

OBJS = main.o console.o player.o gameloop.o

EXE = caterpillar

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS) 
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

console.o: console.c console.h
	$(CC) $(CFLAGS) -c console.c

player.o: player.c player.h globals.h
	$(CC) $(CFLAGS) -c player.c

gameloop.o: gameloop.c gameloop.h player.h globals.h
	$(CC) $(CFLAGS) -c gameloop.c

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)
	rm -f $(EXE)_d

run:
	./$(EXE)

run_debug:
	./$(EXE)_d
