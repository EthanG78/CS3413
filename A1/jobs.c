/*
  jobs.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <jobs.h>
#include <builtins.h>
#include <util.h>

// waitForProcess takes a pid_t stored in pid and waits on the process
// with that process id. waitForProcess stops blocking when the process
// exits or when it pauses via signal.
//
// waitForProcess returns 0 if the process terminated and 1
// if it was stopped via signal.
//
int waitForProcess(pid_t pid)
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

int processPauseLoop(int pid)
{
  char *cwd;              // pointer to current working dir string
  char buffer[INPUT_MAX]; // max input buffer
  char *inputStr;         // pointer to entered cmd
  char **commandArr;      // array of entered commands
  int nCommands;          // number of commands in commandArr
  int execStatus;

  commandArr = (char **)malloc(CMD_MAX * INPUT_MAX);

  do
  {
    cwd = getShellCwd();

    // present the user with the cwd and take input
    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, INPUT_MAX);

    nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, " ");

    execStatus = executeBuiltin(commandArr, nCommands, pid);
    if (execStatus == 0)
    {
      // We only reach this point if a user attempt to exec an external
      // command while the previous command was already stopped by SIGTSTP
      printf("Not allowed to start new command while you have a job active.");
    }

    free(cwd);
  } while (inputStr != NULL && execStatus != -1 && execStatus != pid);

  return execStatus;
}

int spawnProcess(char **cmdArr, int cmdIdx, int nCommands, int *pfds)
{
  int isStopped = 0; // bool to keep track of status of prev process

  int pid = fork();

  int pipeRIdx = (cmdIdx - 1) * 2;
  int pipeWIdx = cmdIdx * 2 + 1;

  if (pid < 0)
  {
    perror("fork()");
  }
  else if (pid == 0)
  {
    // child process

    // redirect the stdout of the command that is about
    // to be executed to the write end of the pipe if
    // there is another command in the pipe array
    if (cmdIdx < nCommands - 1)
    {
      if (dup2(pfds[pipeWIdx], STDOUT_FILENO) == -1)
      {
        perror("dup2()");
      }

      close(pfds[pipeWIdx]);
    }

    // redirect the stdin of the command that is about
    // to be executed to the read end of the pipe if
    // there was another command in the pipe array
    // before this one
    if (cmdIdx > 0)
    {
      if (dup2(pfds[pipeRIdx], STDIN_FILENO) == -1)
      {
        perror("dup2()");
      }

      close(pfds[pipeRIdx]);
    }

    // execute the command with argument
    // array that was created
    if (execvp(cmdArr[0], cmdArr) != 0)
    {
      printf("Error executing %s.\n", cmdArr[0]);
      perror("execvp()");
      exit(EXIT_SUCCESS);
    }
  }

  // close any pipes associated with the
  // process we just ran
  if (cmdIdx > 0)
  {
    // close the associated read end of the
    // pipe that was used as input for this process
    close(pfds[pipeRIdx]);
  }

  if (cmdIdx < nCommands - 1)
  {
    // close the associated write end of the
    // pipe that was used as output for this process
    close(pfds[pipeWIdx]);
  }

  // wait for child process to finish
  isStopped = waitForProcess(pid);

  if (isStopped > 0)
  {
    return processPauseLoop(pid);
  }

  return 1;
}

int executePipeline(char *inputStr)
{
  int i;              // pipe iter
  int isStopped = 0;  // bool to keep track of status of prev process
  int *pfds;          // pipe file descriptors
  int nPipes;         // number of pipes used by user input
  char **commandArr;  // array of entered commands
  int nCommands;      // number of commands in commandArr
  char **argArr;      // array of entered command args
  int nArgs;          // number of args in argArr
  int execStatus = 0; // bool that is returned

  commandArr = (char **)malloc(CMD_MAX * INPUT_MAX);

  // tokenize user input based on the pipe '|' delimeter and
  // iterate through each of the commands the user has entered
  nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, "|");
  if (nCommands > 0)
  {
    argArr = (char **)malloc(CMD_MAX * INPUT_MAX);

    // based on how many commands were entered by the user,
    // we can determine how many pipes were used and how
    // many we must create.
    nPipes = nCommands - 1;
    pfds = createPipes(nPipes);

    // iterate through each command and execute it
    // making sure to modify the stdin and stdout of each
    // process based on what pipe we are on
    for (i = 0; i < nCommands; i++)
    {
      // commandArr[i] becomes the command string we want to split
      nArgs = tokenizeIntoArr(commandArr[i], argArr, CMD_MAX, " ");

      if (nArgs > 0)
      {
        // execute builtins
        execStatus = executeBuiltin(argArr, nArgs, -1);

        if (execStatus == 0)
        {
          // if no builtin was run, run external command
          execStatus = spawnProcess(argArr, i, nCommands, pfds);
          if (execStatus == -1)
          {
            break;
          }
        }
        else
        {
          // if a builtin was run, break from loop
          break;
        }
      }
    }

    free(pfds);
    free(argArr);
  }

  free(commandArr);

  return execStatus;
}