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
#include "gameloop.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  if (initializeGameLoop())
  {
    executeGameLoop();
  }

  return EXIT_SUCCESS;
}
