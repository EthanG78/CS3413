/*
  util.h
  CS3413 Operating Systems 1
  
  Ethan Garnier
*/

#include <sys/types.h>

// todo:
// review these
#define INPUT_MAX 201
#define CMD_MAX 101

int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim); 

//todo:
void waitForProcess(pid_t pid);

