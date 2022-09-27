/*
  util.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <util.h>

// tokenizeIntoArr takes a char* stored in str and splits it into tokens
// based on a character delimeter specified by delim. Each of these tokens
// are stored, in order, in the arr char* array. This array may hold arrSize - 1
// tokens. A NULL pointer is placed at the end of the array for compatibility
// with the exec family of functions.
//
// tokenizeIntoArr returns the integer number of tokens stored in arrSize,
// or -1 to indicate an error.
//
// Thank you to Dr. Rea for inspiring this helper function.
//
int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim)
{
  if (str == NULL) return -1;

  int tokenIdx = 0;
  char *token, *strRemainder;
  strRemainder = str;

  // loop until we no longer have tokens to parse or we have parsed
  // more tokens then we are capable of storing.
  // todo: the man page says str must be set to null after first run...
  while ((token = strtok_r(strRemainder, delim, &strRemainder)) && tokenIdx < arrSize - 1)
  {
    char *nullToken = token + '\0';
    arr[tokenIdx++] = nullToken;
  }

  // add a NULL pointer to the end of the array so it
  // is compatible with the exec() family of functions
  arr[tokenIdx] = (char *)NULL;

  return tokenIdx;
}

// todo:
// waitForProcess takes a pid_t stored in pid and waits on the process
// with that process id. waitForProcess stops blocking when the process
// exits or when it pauses via signal.
//
void waitForProcess(pid_t pid)
{
  int status;
  waitpid(pid, &status, WUNTRACED);

  // todo:
  // debugging stuff
  if (WIFEXITED(status))
  {
    // pid terminated normally
  }
  else if (WIFSIGNALED(status))
  {
    // pid terminated by a signal
  }
}

// getUserInput takes a char* stored in buffer and a max
// size of the buffer and stores user input from stdin
// in the buffer.
//
// getUserInput returns a null-terminated string of the
// entered user input.
//
char *getUserInput(char *buffer, const int maxInput)
{
  char *inputStr;
  int len;

  inputStr = fgets(buffer, maxInput, stdin);

  // check for the newline character and overwrite with \0
  len = strlen(buffer);
  if (buffer[len - 1] == '\n')
  {
    buffer[len - 1] = '\0';
  }

  return inputStr;
}

// getUserCwd returns a pointer to a string containing
// the path of the shell's current working directory.
// The calling function must free the returned pointer
// when they are finished.
//
char *getShellCwd()
{
  // call getcwd() to get the shell's
  // current working directory
  char *cwd = getcwd(NULL, 0);
  if (cwd == NULL)
  {
    perror("getcwd");
  }

  return cwd;
}

// createPipes takes an integer number nPipes and creates
// 2 * nPipes pipe file descriptors (1 read and 1 write
// for aech pipe).
//
// createPipes returns an integer pointer to the
// first of the 2 * nPipes pipe file descriptors, or
// NULL if the function fails to create the pipes
// The calling function must free the returned pointer
// when they are finished.
//
int *createPipes(int nPipes)
{
  // allocate space in the pipe file descriptors
  // variable based on how many pipes there are
  int *pfds = (int *)malloc(nPipes * sizeof(int) * 2);

  // iterate through each of the entered pipes
  // and create a pipe file descriptor
  int i;
  for (i = 0; i < nPipes; i++)
  {
    if (pipe(pfds + i * 2) == -1)
    {
      perror("pipe()");
      return NULL;
    }
  }

  return pfds;
}


// todo
// parseUserInput takes ...
//
// parseUserInput returns either 0 to indicate
// to the calling function to continue parsing 
// user input or -1 to indicate to the calling
// function to stop parsing user input.
// 
int parseUserInput(char *userInput)
{
  int returnCode = 0;

  char *commandArr = (char **)malloc(CMD_MAX * INPUT_MAX);
  char *cmdArr = (char **)malloc(CMD_MAX * INPUT_MAX);


  free(commandArr);
  free(cmdArr);
  return returnCode;
}
