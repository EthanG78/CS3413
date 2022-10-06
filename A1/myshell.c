/*
  myshell.c
  CS3413 Operating Systems 1

  To compile with Makefile run
  > make

  To run the compiled binary call
  > ./myshell

  Ethan Garnier
*/
#include <util.h>
#include <jobs.h>

int main(int argc, char *argv[])
{
  char *cwd;                                 // pointer to current working dir string
  char *inputStr;                            // pointer to entered cmd
  char buffer[CMD_MAX * ARG_MAX * CHAR_MAX]; // max input buffer
  int execStatus = 1;                        // bool to keep track of the status of the shell.

  // ISSUES:
  // - the cwd will not print out directly after SIGTSTP is captured
  //   if it is captured while user input is being taken...

  if (subscribeToSignals() == -1)
  {
    return EXIT_FAILURE;
  }

  do
  {
    jobPid = 0;

    // fetch the shell's current working directory
    cwd = getShellCwd();

    // fetch user input
    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, CMD_MAX * ARG_MAX * CHAR_MAX);

    // supply the user input to the executePipeline
    // command to execute whatever combination of
    // commands were entered by the user.
    execStatus = executePipeline(inputStr);

    // since we aquire the cwd on each
    // loop we must then free it on each loop
    free(cwd);
  } while (inputStr != NULL && execStatus != -1);

  return EXIT_SUCCESS;
}
