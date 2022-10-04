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
#define CMD_MAX 100
// The maximum amount of arguments 
// accepted per command
#define ARG_MAX 101
// The maximum amount of characters
// allowed per argument
#define CHAR_MAX 50

int waitForProcess(int pid);

int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim);

char *getUserInput(char *buffer, const int maxInput);

char *getShellCwd();

int *createPipes(int nPipes);