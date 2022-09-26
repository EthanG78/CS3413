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
#include <math.h>
#include <util.h>

int main(int argc, char *argv[])
{
  char *cwd;              // pointer to current working dir string
  char *inputStr;         // pointer to entered cmd
  char **commandArr;      // arry of entered commands
  int nCommands;          // number of commands in pipeArr
  int nPipes;             // number of pipes used by user input
  char **cmdArr;          // array of entered cmd args
  int nArgs;              // number of args in cmdArr
  char buffer[INPUT_MAX]; // max input buffer
  pid_t pid;              // process id
  int i;                  // pipe iter
  int *pfds;              // pipe file descriptors
  int pipeRIdx;           // current pipe read index
  int pipeWIdx;           // current pipe write index

  // review the amount of data we malloc
  commandArr = (char **)malloc(CMD_MAX * INPUT_MAX);
  cmdArr = (char **)malloc(CMD_MAX * INPUT_MAX);

  // call getcwd() to get the shell's
  // current working directory
  cwd = getcwd(NULL, 0);
  if (cwd == NULL)
  {
    perror("getcwd");
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
    // iterate through each of the commands the user has entered
    nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, "|");

    // TODO:

    // Here is where we will handle builtin functions
    // ..

    // based on how many commands were entered by the user,
    // we can determine how many pipes were used
    nPipes = nCommands - 1;

    // allocate space in the pipe file descriptors
    // variable based on how many pipes there are
    pfds = (int *)malloc(nPipes * sizeof(int) * 2);

    // iterate through each of the entered pipes
    // and create a pipe file descriptor
    for (i = 0; i < nPipes; i++)
    {
      if (pipe(pfds + i * 2) == -1)
      {
        perror("pipe()");
        return EXIT_FAILURE;
      }
    }

    // iterate through each command and execute it
    // making sure to modify the stdin and stdout of each
    // process based on what pipe we are on
    for (i = 0; i < nCommands; i++)
    {
      // commandArr[i] becomes the command string we want to split
      nArgs = tokenizeIntoArr(commandArr[i], cmdArr, CMD_MAX, " ");

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

          pipeRIdx = (i - 1) * 2;
          pipeWIdx = i * 2 + 1;

          if (pid < 0)
          {
            perror("fork()");
            return EXIT_FAILURE;
          }
          else if (pid == 0)
          {
            // todo:
            // PIPES WORK!!!!
            // however there must be a pfd
            // i am not closing because the program
            // hangs after. probably waiting
            // on input pipe....
            // SOLUTION: we need to close pfd
            // in the main process BEFORE we wait
            // but will this mess up the forks??

            // redirect the stdout of the command that is about
            // to be executed to the write end of the pipe if
            // there is another command in the pipe array
            if (i < nCommands - 1)
            {
              if (dup2(pfds[pipeWIdx], STDOUT_FILENO) == -1)
              {
                perror("dup2()");
                return EXIT_FAILURE;
              }
            }

            // redirect the stdin of the command that is about
            // to be executed to the read end of the pipe if
            // there was another command in the pipe array
            // before this one
            if (i > 0)
            {
              if (dup2(pfds[pipeRIdx], STDIN_FILENO) == -1)
              {
                perror("dup2()");
                return EXIT_FAILURE;
              }
            }

            // close both pipe file descriptors since
            // they have either been duped to stdin/out
            // or they are not being used
            close(pfds[pipeRIdx]);
            close(pfds[pipeWIdx]);

            // execute the command with argument
            // array that was created
            if (execvp(cmdArr[0], cmdArr) != 0)
            {
              printf("Error executing %s.\n", cmdArr[0]);
              perror("execvp()");
              exit(EXIT_SUCCESS);
            }
          }

          // close any pipes associated with the
          // process we just ran
          if (i > 0)
          {
            // close the associated read end of the
            // pipe that was used as input for this process
            close(pfds[pipeRIdx]);
          }

          if (i < nCommands - 1)
          {
            // close the associated write end of the
            // pipe that was used as output for this process
            close(pfds[pipeWIdx]);
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

  // free the allocated space for arrays
  free(commandArr);
  free(cmdArr);

  // since cwd is dynamically allocated
  // we must call free on it
  free(cwd);

  // free the allocated space
  // for pipe file descriptors
  free(pfds);

  return EXIT_SUCCESS;
}
