/*
  signHandlers.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <signal.h>
#include <sigHandlers.h>

// parentHandler acts as the signal handler for the 
// parent shell process. Its purpose is to tell the
// user there is no job to suspend if they have
// sent the SIGTSTP signal to the shell.
//
// getUserInput returns a null-terminated string of the
// entered user input.
//
void parentHandler(int signum)
{
    printf("\nNo job to suspend\n");
    signal(SIGTSTP, &parentHandler);
} 