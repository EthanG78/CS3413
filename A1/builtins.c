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
        kill(pid, SIGCONT);
        isStopped = waitForProcess(pid);

        return (isStopped == 0) ? pid : 1;
    }
    else
    {
        printf("No job to continue.\n");
        return 1;
    }
}

int bg(int pid)
{
    // if previous job was stopped, bring it back to
    // life but do not pause the shell
    if (pid != -1)
    {
        kill(pid, SIGCONT);
    }
    else
    {
        printf("No job to continue.\n");
    }

    return 1;
}

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
