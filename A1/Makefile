SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

FLAGS = -Wall

all: a1

a1:
	gcc myshell.c -o myshell ${FLAGS}

clean:
	rm -rvf *.exe *.o ${BINS}
