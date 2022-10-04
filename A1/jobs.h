/*
  jobs.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/

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
int executePipeline(char *inputStr);
