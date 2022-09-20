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
  char *cwd;              // pointer to current working dir string
  char *inputStr;         // pointer to entered cmd
  char **cmdArr;          // array of entered cmd args
  int nArgs;              // number of args in cmdArr
  char buffer[INPUT_MAX]; // max input buffer
  int len;                // length of entered command

  cmdArr = (char **)malloc(CMD_MAX * INPUT_MAX);

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
  while (inputStr != NULL)
  {
    // check for the newline character and overwrite with \0
    len = strlen(buffer);
    if (buffer[len - 1] == '\n')
    {
      buffer[len - 1] = '\0';
    }

    if (strcmp(inputStr, "exit") == 0)
    {
      break;
    }

    nArgs = tokenizeIntoArr(inputStr, cmdArr, CMD_MAX, " ");

    // check for user input
    if (nArgs > 0)
    {
      if (strcmp(cmdArr[0], "cd") == 0)
      {
        // manually implement cd with chdir
        if (chdir(cmdArr[1]) == -1)
        {
          perror("chdir()");
        }

        // make sure to update cwd var
        cwd = getcwd(NULL, 0);
        if (cwd == NULL)
        {
          perror("getcwd");
        }
      }
    }

    int i;
    for (i = 0; i < nArgs; i++)
    {
      printf("%s\n", cmdArr[i]);
    }

    printf("%s%%", cwd);
    inputStr = fgets(buffer, INPUT_MAX, stdin);
  }

  printf("Exiting shell\n");

  // free the allocated space for array of
  // entered command args
  free(cmdArr);

  // since cwd is dynamically allocated
  // we must call free on it
  free(cwd);

  return 0;
}
