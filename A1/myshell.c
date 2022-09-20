/*
  myshell.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <util.h>

int main(int argc, char *argv[])
{
  char *cwd;                // pointer to current working dir string
  char *inputStr;           // pointer to entered cmd
  char *cmdArr[INPUT_MAX];  // array of entered cmd args
  char buffer[INPUT_MAX];   // max input buffer
  int len;                  // length of entered command

  cmdArr = malloc(CMD_MAX);

  // call getcwd() to get the shell's
  // current working directory
  cwd = getcwd(NULL, 0);
  if (cwd == NULL)
  {
    perror("getcwd");
  }

  // present the user with the cwd and take input
  printf("%s%%", cwd);
  inputStr = fgets(buffer, INPUT_MAX, stdin);

  // this is the main shell loop which will accept
  // user input, handle all errors, and end the
  // program when the user enters 'exit'
  while (inputStr != NULL || strcmp(inputStr, "exit") != 0)
  {
    // check for the newline character and overwrite with \0
    len = strlen(buffer);
    if (buffer[len - 1] == '\n')
    {
      buffer[len - 1] = '\0';
    }

    int nArgs = tokenizeIntoArr(inputStr, cmdArr, CMD_MAX, " ");
    int i;
    for (i = 0; i < nArgs; i++)
    {
      printf("%s\n", cmdArr[i]);
    }

    printf("%s%%", cwd);
    inputStr = fgets(buffer, INPUT_MAX, stdin);
  }

  // free the allocated space for array of
  // entered command args
  free(cmdArr);

  // since cwd is dynamically allocated
  // we must call free on it
  free(cwd);

  return 0;
}
