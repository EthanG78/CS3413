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
#include <signal.h>
#include <util.h>
#include <sigHandlers.h>

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

  do
  {
    cwd = getShellCwd();

    // present the user with the cwd and take input
    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, INPUT_MAX);

    // todo:
    // what if we moved all of this into its own function
    // that returns in integer indicating exit or continue loop?
    // int cont = parseUserInput(inputStr);

    // tokenize user input based on the pipe '|' delimeter and
    // iterate through each of the commands the user has entered
    nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, "|");
    if (nCommands > 0)
    {
      // based on how many commands were entered by the user,
      // we can determine how many pipes were used and how
      // many we must create.
      nPipes = nCommands - 1;
      pfds = createPipes(nPipes);

      // iterate through each command and execute it
      // making sure to modify the stdin and stdout of each
      // process based on what pipe we are on
      for (i = 0; i < nCommands; i++)
      {
        // commandArr[i] becomes the command string we want to split
        nArgs = tokenizeIntoArr(commandArr[i], cmdArr, CMD_MAX, " ");

        // todo:
        // handle the builtins here
        // this is temp code I hate this
        if (strcmp(inputStr, "exit") == 0)
        {
          free(cwd);
          free(pfds);

          // break out of all loops
          // to go to memory cleanup
          // code before exiting
          goto cleanup;
        }
        else if (strcmp(cmdArr[0], "cd") == 0)
        {
          if (nArgs > 1)
          {
            // manually implement cd with chdir
            if (chdir(cmdArr[1]) == -1)
            {
              perror("chdir()");
            }
          }
          else
          {
            printf("Please supply a directory with the CD command\n");
          }
        }
        else
        {
          // here is where we will execute external commands
          // todo:
          // do I need to return EXIT_FAILURE on some of these??
          pid = fork();

          pipeRIdx = (i - 1) * 2;
          pipeWIdx = i * 2 + 1;

          if (pid < 0)
          {
            perror("fork()");
          }
          else if (pid == 0)
          {
            // redirect the stdout of the command that is about
            // to be executed to the write end of the pipe if
            // there is another command in the pipe array
            if (i < nCommands - 1)
            {
              if (dup2(pfds[pipeWIdx], STDOUT_FILENO) == -1)
              {
                perror("dup2()");
              }

              close(pfds[pipeWIdx]);
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
              }

              close(pfds[pipeRIdx]);
            }

            // execute the command with argument
            // array that was created
            if (execvp(cmdArr[0], cmdArr) != 0)
            {
              printf("Error executing %s.\n", cmdArr[0]);
              perror("execvp()");
              exit(EXIT_SUCCESS);
            }
          }

          // Subscribe to SIGTSTP
          signal(SIGTSTP, &parentHandler);

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

    // since we aquire the cwd and pfds on each
    // loop we must then free it on each loop
    free(cwd);
    free(pfds);
  } while (inputStr != NULL);

cleanup:

  // free all allocated memory
  free(commandArr);
  free(cmdArr);

  return EXIT_SUCCESS;
}
