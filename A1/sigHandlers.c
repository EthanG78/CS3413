/*
  signHandlers.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <sigHandlers.h>

// parentHandler acts as the signal handler for the
// parent shell process. 
//
void parentHandler(int signum)
{
  switch (signum)
  {
  case SIGTSTP:
    printf("\nNo job to suspend\n");
    signal(SIGTSTP, &parentHandler);
    break;
  default:
    break;
  }
}