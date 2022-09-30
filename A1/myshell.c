/*
  myshell.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/
#include <util.h>
#include <sigHandlers.h>
#include <jobs.h>

int main(int argc, char *argv[])
{
  char *cwd;              // pointer to current working dir string
  char *inputStr;         // pointer to entered cmd
  char buffer[INPUT_MAX]; // max input buffer
  int execStatus = 1;     // bool to keep track of the status of the shell.

  // Subscribe to SIGTSTP
  // BUG:
  // currently the cwd will not print out directly
  // after SIGTSTP is captured by main process...
  signal(SIGTSTP, &parentHandler);

  // ISSUES:
  // - "No job to suspend" is printed when we suspend a child process
  // - Suspending a process with pipes is a nightmare and breaks

  do
  {
    // fetch the shell's current working directory
    cwd = getShellCwd();

    // present the user with the cwd and take input
    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, INPUT_MAX);

    // supply the user input to the executePipeline
    // command to execute whatever combination of
    // commands were entered by the user.
    // execStatus = 0 if no command was run
    // execStatus = 1 if a command was run
    // execStatus = -1 if exit was run
    execStatus = executePipeline(inputStr);

    // since we aquire the cwd on each
    // loop we must then free it on each loop
    free(cwd);
  } while (inputStr != NULL && execStatus != -1);

  return EXIT_SUCCESS;
}
