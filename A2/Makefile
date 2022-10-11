CC = gcc

BASEFLAGS = -Wall -pthread -std=c99
NODEBUG_FLAGS = -dNDEBUG 
DEBUG_FLAGS = -g

LDLIBS = -lcurses -pthread

OBJS = main.o console.o	

debug: CFLAGS = $(BASEFLAGS) $(DEBUG_FLAGS)
debug: EXE = caterpillar_d
debug: $(EXE)

release: CFLAGS = $(BASEFLAGS) $(NODEBUG_FLAGS)
debug: EXE = caterpillar
release: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

console.o: console.c console.h
	$(CC) $(CFLAGS) -c console.c

clean:
	rm -f $(OBJS)
	rm -f *~
	rm -f $(EXE)
	rm -f $(EXE)_d

run:
	./$(EXE)

run_debug:
	./$(EXE)_d
