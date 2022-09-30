/*
  builtins.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <builtins.h>
#include <util.h>

void cd(char **argArr, int nArgs)
{
    if (nArgs > 1)
    {
        // manually implement cd with chdir
        if (chdir(argArr[1]) == -1)
        {
            perror("chdir()");
        }
    }
    else
    {
        printf("Please supply a directory with the CD command\n");
    }
}

/*
else if (strcmp(cmdArr[0], "fg") == 0)
        {
          // if previous job was stopped, bring it back to
          // life and pause the shell.
          if (isStopped == 1)
          {
            // pid of previous job is preserved
            kill(pid, SIGCONT);

            // wait for child process to finish
            // since we brought it back to foreground
            isStopped = waitForProcess(pid);

            // alert the user if child process was stopped
            if (isStopped > 0)
            {
              printf("Job suspended. Type 'fg' to resume\n");
            }
          }
          else
          {
            printf("No job to continue.\n");
          }
        }
        else if (strcmp(cmdArr[0], "bg") == 0)
        {
          // BROKEN BROKEN

          // if previous job was stopped, bring it back to
          // life but do not pause the shell
          if (isStopped == 1)
          {
            // pid of previous job is preserved
            kill(pid, SIGCONT);
          }
          else
          {
            printf("No job to continue.\n");
          }
        }
*/

// todo:
// returns 0 if no builtin was run
// returns -1 if exit was run
// returns 1 if any other builtin was run
int executeBuiltin(char **argArr, int nArgs)
{
    int retCode = 0;
    if (strcmp(argArr[0], "exit") == 0)
    {
        // call exit
        retCode = -1;
    }
    else if (strcmp(argArr[0], "cd") == 0)
    {
        // call cd
        cd(argArr, nArgs);
        retCode = 1;
    }

    return retCode;
}
