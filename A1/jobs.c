/*
  jobs.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <errno.h>
#include <signal.h>
#include <jobs.h>
#include <builtins.h>
#include <util.h>

// processPauseLoop represents a shell within a shell.
// I do not like this, but this is my solution to preserving
// pipeline integrety while we have stopped a process in the
// pipeline. This internal shell will allow users to execute
// builtin functions, but not external functions. They must finish
// the process that was stopped by using 'fg' or 'bg' before
// the pipeline resumes.
//
// processPauseLoop returns an integer value greater than
// 0 to indicate some process executed successfully, or
// -1 to indicate we must exit the shell.
//
int processPauseLoop(int pid)
{
  char *cwd;                                 // pointer to current working dir string
  char buffer[CMD_MAX * ARG_MAX * CHAR_MAX]; // max input buffer
  char *inputStr;                            // pointer to entered cmd
  char **commandArr;                         // array of entered commands
  int nCommands;                             // number of commands in commandArr
  int execStatus = 0;                        // maintain status of commands being run

  commandArr = (char **)malloc(CMD_MAX * ARG_MAX * CHAR_MAX * sizeof(char));

  do
  {
    cwd = getShellCwd();

    printf("%s%%", cwd);
    inputStr = getUserInput(buffer, CMD_MAX * ARG_MAX * CHAR_MAX);

    nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, " ");

    if (nCommands > 0)
    {
      execStatus = executeBuiltin(commandArr, nCommands, pid);
      if (execStatus == 0)
      {
        // We only reach this point if a user attempt to exec an external
        // command while the previous command was already stopped by SIGTSTP
        printf("Not allowed to start new command while you have a job active.");
      }
    }

    free(cwd);
  } while (inputStr != NULL && execStatus != -1 && execStatus != pid);

  return execStatus;
}

// spawnProcess takes an array of command arguments, the
// current index of the command in the pipeline, the total
// number of commands in the pipeline, and the pipe file
// descriptors for the pipeline and spawns a new process to
// run the particular command in cmdArr. spawnProcess will
// use the information about the pipeline to ensure the
// input and output of the command in cmdArr are piped to
// the correct location.
//
// spawnProcess returns an integer value greater than
// 0 to indicate some process executed successfully, or
// -1 to indicate we must exit the shell.
//
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

// executePipeline takes a pointer to the user input characters
// and executes the pipeline of commands that were
// entered by the user. This includes builtin commands
// and external commands.
//
// executePipeline returns an integer value which
// indicates the status of what was executed.
// execStatus = 0 if no command was run
// execStatus > 0 if a command was run
// execStatus = -1 if exit was run
//
int executePipeline(char *inputStr)
{
  int i;              // pipe iter
  int *pfds;          // pipe file descriptors
  int nPipes;         // number of pipes used by user input
  char **commandArr;  // array of entered commands
  int nCommands;      // number of commands in commandArr
  char **argArr;      // array of entered command args
  int nArgs;          // number of args in argArr
  int execStatus = 0; // bool that is returned

  // allocate space for each command, this is equivalent
  // to the number of commands we allow times the number
  // of arguments we allow per command times the number
  // of character per argument we support.
  commandArr = (char **)malloc(CMD_MAX * ARG_MAX * CHAR_MAX * sizeof(char));

  // tokenize user input based on the pipe '|' delimeter and
  // iterate through each of the commands the user has entered
  nCommands = tokenizeIntoArr(inputStr, commandArr, CMD_MAX, "|");
  if (nCommands > 0)
  {
    // allocate space for each argument, this is equivalent
    // to the numberof arguments we allow per command
    // times the number of character per argument we support.
    argArr = (char **)malloc(ARG_MAX * CHAR_MAX * sizeof(char));

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
      nArgs = tokenizeIntoArr(commandArr[i], argArr, ARG_MAX, " ");

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