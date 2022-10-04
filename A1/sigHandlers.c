/*
  signHandlers.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <stdio.h>
#include <sigHandlers.h>

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