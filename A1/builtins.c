/*
  builtins.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <string.h>
#include <signal.h>
#include <builtins.h>
#include <util.h>

// change directory
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

// resume child process in foreground
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

// resume child process in background
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

// executeBuiltin takes a pointer to the array of command
// arguments supplied by the user, an integer with the number
// of arguments in the array, and an optional integer process id.
// executeBuiltin will execute any supported
// builtin command if found in the arguments array.
//
// The builtin commands for resuming paused processes in the
// foreground (fg) and background (bg) will only work if the
// paused process id is supplied in pid.
//
// executeBuiltin returns an integer value which
// indicates the status of what was executed.
// returns 0 if no builtin was run
// returns -1 if exit was run
// returns > 0 if any builtin was run
//
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
