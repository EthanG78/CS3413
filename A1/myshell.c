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
  char **pipeArr;         // arry of entered pipes
  int nPipes;             // number of pipes in pipeArr;
  char **cmdArr;          // array of entered cmd args
  int nArgs;              // number of args in cmdArr
  char buffer[INPUT_MAX]; // max input buffer
  int len;                // length of entered command
  pid_t pid;              // process id
  int i;                  // pipe iter

  // review the amount of data we malloc
  pipeArr = (char **)malloc(CMD_MAX * INPUT_MAX);
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

    // todo:
    // what if we call this with '|' as the delimeter first
    // so we can fetch all of the pipes, and then iterate through
    // each one calling the command on that side of the pipe!!!!
    nPipes = tokenizeIntoArr(inputStr, pipeArr, CMD_MAX, "|");
    for (i = 0; i < nPipes; i++)
    {
      // pipeArr[i] becomes the command string we want to split
      nArgs = tokenizeIntoArr(pipeArr[i], cmdArr, CMD_MAX, " ");

      if (nArgs > 0)
      {
        // todo:
        // i don't think
        // we should assume it is always
        // the first command entered,
        // however that appears to be how
        // the test program works.
        // IS THERE A BETTER WAY OF IMPLM
        // INTERNAL COMMANDS????
        if (strcmp(cmdArr[0], "cd") == 0)
        {
          if (nArgs > 1)
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
              perror("getcwd()");
            }
          }
          else
          {
            printf("A directory to cd into must be supplied with the cd command.\n");
          }
        }
        else
        {
          // todo:
          // executing commands
          pid = fork();
          if (pid != 0)
          {
            // wait for child process to finish
            waitForProcess(pid);
          }
          else
          {
            // execute the command with argument
            // array that was created
            if (execvp(cmdArr[0], cmdArr) != 0)
            {
              printf("Error executing %s.\n", cmdArr[0]);
              perror("execvp()");
              exit(0);
            }
          }
        }
      }
    }

    printf("%s%%", cwd);
    inputStr = fgets(buffer, INPUT_MAX, stdin);
  }

  printf("Exiting shell\n");

  // free the allocated space for arrays
  free(pipeArr);
  free(cmdArr);

  // since cwd is dynamically allocated
  // we must call free on it
  free(cwd);

  return 0;
}
