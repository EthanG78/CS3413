/*
  main.c
  CS3413 Operating Systems 1
  Assignment 2
  FALL 2022

  To compile with Makefile run
  > make

  To run the compiled binary call
  > ./caterpillar

  Author: Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include "gameloop.h"

int main(int argc, char **argv)
{
  // Attempt to initialize the main gameloop,
  // and execute the main gameloop if successful.
  if (initializeGameLoop())
    executeGameLoop();

  // Once the gameloop is done running, attempt
  // to cleanup what remains.
  if (!cleanupGameLoop())
  {
    // If we are unable to cleanup the gameloop,
    // we must alert the user and return failure.
    printf("Error cleaning up game loop.\n");
    return EXIT_FAILURE;
  }

  printf("done!\n");
  return EXIT_SUCCESS;
}
