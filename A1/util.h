/*
  util.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// todo:
// review these
#define INPUT_MAX 51
#define CMD_MAX 100

int waitForProcess(int pid);

int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim);

char *getUserInput(char *buffer, const int maxInput);

char *getShellCwd();

int *createPipes(int nPipes);