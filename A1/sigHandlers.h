/*
  signHandlers.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <signal.h>

// parentHandler acts as the signal handler for the
// parent shell process. Whenever the shell wishes 
// to handle a particular signal in the parent process,
// it shell be handled here.
//
void parentHandler(int signum);