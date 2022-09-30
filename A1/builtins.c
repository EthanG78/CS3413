/*
  builtins.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <string.h>
#include <signal.h>
#include <builtins.h>
#include <util.h>

int cd(char **argArr, int nArgs)
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

    return 1;
}

int fg(int pid)
{
    int isStopped = 0;

    // if previous job was stopped, bring it back to
    // life and pause the shell.
    if (pid != -1)
    {
        // pid of previous job is preserved
        kill(pid, SIGCONT);

        // wait for child process to finish
        // since we brought it back to foreground
        isStopped = waitForProcess(pid);

        return (isStopped == 0) ? pid : 1;
    }
    else
    {
        printf("No job to continue.\n");
        return 1;
    }
}

// BROKEN BROKEN
int bg(int pid)
{
    // BROKEN BROKEN

    // if previous job was stopped, bring it back to
    // life but do not pause the shell
    if (pid != -1)
    {
        // pid of previous job is preserved
        kill(pid, SIGCONT);
    }
    else
    {
        printf("No job to continue.\n");
    }

    return 1;
}

// todo:
// returns 0 if no builtin was run
// returns -1 if exit was run
// returns > 0 if any builtin was run
int executeBuiltin(char **argArr, int nArgs, int pid)
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
        retCode = cd(argArr, nArgs);
    }
    else if (strcmp(argArr[0], "fg") == 0)
    {
        // call fg
        retCode = fg(pid);
    }
    else if (strcmp(argArr[0], "bg") == 0)
    {
        // call bg
        retCode = bg(pid);
    }

    return retCode;
}
