/*
  builtins.h
  CS3413 Operating Systems 1

  Ethan Garnier
*/

// executeBuiltin takes a pointer to the array of command
// arguments supplied by the user, an integer with the number
// of arguments in the array, and an optional integer process id.
// executeBuiltin will execute any supported
// builtin command if found in the arguments array.
//
// The builtin commands for resuming paused processes in the
// foreground (fg) and background (bg) will only work if the
// paused process id is supplied in pid.
//
// executeBuiltin returns an integer value which
// indicates the status of what was executed.
// returns 0 if no builtin was run
// returns -1 if exit was run
// returns > 0 if any builtin was run
//
int executeBuiltin(char **argArr, int nArgs, int pid);
