/*
  signHandlers.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <sigHandlers.h>

void parentHandler(int signum)
{
  signal(signum, &parentHandler);

  switch (signum)
  {
  case SIGTSTP:
    // I know using printf here is bad because 
    // it is not an 'async-signal-safe' function.
    printf("\nNo job to suspend\n");
    fputs("\n", stdin);
    break;
  default:
    break;
  }
}