/*
  util.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <util.h>

// waitForProcess takes an integer process id stored in pid and
// waits on the process with that process id. waitForProcess stops
// blocking when the process exits or when it pauses via signal.
//
// waitForProcess returns 0 if the process terminated and 1
// if it was stopped via signal.
//
int waitForProcess(int pid)
{
  int status;
  waitpid(pid, &status, WUNTRACED);

  if (WIFSTOPPED(status))
  {
    printf("Job suspended. Type 'fg' to resume\n");
    return 1;
  }
  else
  {
    return 0;
  }
}

// tokenizeIntoArr takes a char* stored in str and splits it into tokens
// based on a character delimeter specified by delim. Each of these tokens
// are stored, in order, in the arr char* array. This array may hold arrSize - 1
// tokens. A NULL pointer is placed at the end of the array for compatibility
// with the exec family of functions.
//
// tokenizeIntoArr returns the integer number of tokens stored in arrSize,
// or -1 to indicate an error or no input.
//
// Thank you to Dr. Rea for inspiring this helper function.
//
int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim)
{
  if (str == NULL)
    return -1;

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

  // in the event the user entered more
  // input then we support
  if (tokenIdx == arrSize - 1 && token != NULL)
  {
    // todo:
    // decide if we should just return what has already
    // been tokenized, or return -1
    //
    // my initial thought process is that if the user
    // cannot execute their entire pipeline then they
    // probably don't want to execute a portion of it
    // so just return -1 for now...
    printf("Unsupported size of input...\nShell only supports %d commands with %d arguments each.\n", CMD_MAX, ARG_MAX);
    return -1;
  }

  // add a NULL pointer to the end of the array so it
  // is compatible with the exec() family of functions
  arr[tokenIdx] = (char *)NULL;

  return tokenIdx;
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
// for each pipe).
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

// sigHandler represents the signal handler
// called when process is subscribed to a 
// a particular signal.
//
void sigHandler(int signum)
{
  // Resubscribe to signal handler
  signal(signum, &sigHandler);

  // Add signals to handle here
  switch (signum)
  {
  case SIGTSTP:
    // I know using printf here is bad because
    // it is not an 'async-signal-safe' function.
    if (jobPid == 0)
    {
      printf("\nNo job to suspend\n");
    }
    break;
  default:
    break;
  }
}

// subscribeToSignals subscribes a process
// to all desired signals with handlers.
//
// subscribeToSignals return -1 if an error occured,
// and 0 otherwise.
//
int subscribeToSignals()
{
  if (signal(SIGTSTP, &sigHandler) == SIG_ERR)
  {
    perror("signal()");
    return -1;
  }

  return 0;
}