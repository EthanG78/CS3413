/*
  util.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <sys/types.h>

// todo:
// review these
#define INPUT_MAX 51
#define CMD_MAX 100

int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim);

int waitForProcess(pid_t pid);

char *getUserInput(char *buffer, const int maxInput);

char *getShellCwd();

int *createPipes(int nPipes);

int parseUserInput(char *userInput);
