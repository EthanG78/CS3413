/*
  util.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The maximum amount of commands
// accepted to be piped together
#define CMD_MAX 101
// The maximum amount of arguments
// accepted per command
#define ARG_MAX 101
// The maximum amount of characters
// allowed per argument
#define CHAR_MAX 50

// global child process id to keep track
// of which process is our immediate child
int childPid;

// waitForProcess takes an integer process id stored in pid and
// waits on the process with that process id. waitForProcess stops
// blocking when the process exits or when it pauses via signal.
//
// waitForProcess returns 0 if the process terminated and 1
// if it was stopped via signal.
//
int waitForProcess(int pid);

// tokenizeIntoArr takes a char* stored in str and splits it into tokens
// based on a character delimeter specified by delim. Each of these tokens
// are stored, in order, in the arr char* array. This array may hold arrSize - 1
// tokens. A NULL pointer is placed at the end of the array for compatibility
// with the exec family of functions.
//
// tokenizeIntoArr returns the integer number of tokens stored in arrSize,
// or -1 to indicate an error or no input.
//
// Thank you to Dr. Rea for inspiring this helper function.
//
int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim);

// getUserInput takes a char* stored in buffer and a max
// size of the buffer and stores user input from stdin
// in the buffer.
//
// getUserInput returns a null-terminated string of the
// entered user input.
//
char *getUserInput(char *buffer, const int maxInput);

// getUserCwd returns a pointer to a string containing
// the path of the shell's current working directory.
// The calling function must free the returned pointer
// when they are finished.
//
char *getShellCwd();

// createPipes takes an integer number nPipes and creates
// 2 * nPipes pipe file descriptors (1 read and 1 write
// for each pipe).
//
// createPipes returns an integer pointer to the
// first of the 2 * nPipes pipe file descriptors, or
// NULL if the function fails to create the pipes
// The calling function must free the returned pointer
// when they are finished.
//
int *createPipes(int nPipes);

// sigHandler acts as the signal handler for the
// shell process and its children.
//
void sigHandler(int signum);
