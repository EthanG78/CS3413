CC = gcc

BASEFLAGS = -Wall -pthread -std=c99 -D_XOPEN_SOURCE=700
DEBUG_FLAGS = -g

LDLIBS = -lcurses -pthread -lm

OBJS = main.o console.o bullet.o player.o enemy.o gameloop.o

EXE = caterpillar

release: CFLAGS = $(BASEFLAGS)
release: $(EXE)

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

console.o: console.c console.h
	$(CC) $(CFLAGS) -c console.c

bullet.o: bullet.c bullet.h globals.h
	$(CC) $(CFLAGS) -c bullet.c

player.o: player.c player.h globals.h
	$(CC) $(CFLAGS) -c player.c

enemy.o: enemy.c enemy.h globals.h
	$(CC) $(CFLAGS) -c enemy.c

gameloop.o: gameloop.c gameloop.h player.h globals.h
	$(CC) $(CFLAGS) -c gameloop.c

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)
	rm -f $(EXE)_d

run:
	./$(EXE)
