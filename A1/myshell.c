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
  pid_t pid;              // process id
  int i;                  // pipe iter
  int pfd[2];             // pipe file descriptors

  // review the amount of data we malloc
  pipeArr = (char **)malloc(CMD_MAX * INPUT_MAX);
  cmdArr = (char **)malloc(CMD_MAX * INPUT_MAX);

  // call getcwd() to get the shell's
  // current working directory
  cwd = getcwd(NULL, 0);
  if (cwd == NULL)
  {
    perror("getcwd");
    return EXIT_FAILURE;
  }

  // open pipe file descriptors
  if (pipe(pfd) == -1)
  {
    perror("pipe()");
    return EXIT_FAILURE;
  }

  // present the user with the cwd and take input
  printf("%s%%", cwd);
  inputStr = getUserInput(buffer, INPUT_MAX);

  // this is the main shell loop which will accept
  // user input, handle all errors, and end the
  // program when the user enters 'exit' or
  // presses CTRL + D
  while (inputStr != NULL)
  {
    if (strcmp(inputStr, "exit") == 0)
    {
      break;
    }

    // tokenize user input based on the pipe '|' delimeter and
    // iterate through each of the pipes the user has entered
    nPipes = tokenizeIntoArr(inputStr, pipeArr, CMD_MAX, "|");
    for (i = 0; i < nPipes; i++)
    {
      // todo:
      // when redirecting stdout, we can check if
      // i = nPipes - 1, if not then redirect stdout
      // when redirecting stdin, we can check if i > 0,
      // if so then redirect stdin to pipe.

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

          if (pid < 0)
          {
            perror("fork()");
            return EXIT_FAILURE;
          }
          else if (pid == 0)
          {
            // redirect the stdout of the command that is about
            // to be executed to the write end of the pipe if
            // there is another command in the pipe array
            if (i < nPipes - 1)
            {
              dup2(pfd[1], STDOUT_FILENO);
            }

            // redirect the stdin of the command that is about
            // to be executed to the read end of the pipe if
            // there was another command in the pipe array
            // before this one
            if (i > 0)
            {
              dup2(pfd[0], STDIN_FILENO);
            }

            // close both pipe file descriptors since
            // they have either been duped to stdin/out
            // or they are not being used
            close(pfd[0]);
            close(pfd[1]);

            // execute the command with argument
            // array that was created
            if (execvp(cmdArr[0], cmdArr) != 0)
            {
              printf("Error executing %s.\n", cmdArr[0]);
              perror("execvp()");
              exit(EXIT_SUCCESS);
            }
          }

          // wait for child process to finish
          waitForProcess(pid);
        }
      }
    }

    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, INPUT_MAX);
  }

  printf("Exiting shell\n");

  // close pipe file descriptors
  close(pfd[0]);
  close(pfd[1]);

  // free the allocated space for arrays
  free(pipeArr);
  free(cmdArr);

  // since cwd is dynamically allocated
  // we must call free on it
  free(cwd);

  return EXIT_SUCCESS;
}
