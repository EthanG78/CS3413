/*
  util.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <util.h>

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
    if (childPid != 0)
    {
      printf("\nNo job to suspend\n");
    }
    break;
  default:
    break;
  }
}