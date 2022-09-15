/*
  util.c
  COMP 3413 Operating Systems 1
  
  Ethan Garnier
*/

#include <string.h>

// todo:
// fully comment this function
// it returns the number of tokens
// and credit dr. rea
int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim)
{
  // plan:
  // use an index to keep track of how many
  // tokens we have storred in arr, and return
  // this number
  
  int tokenIdx = 0;
  char *token, *strRemainder;
  strRemainder = str;

  // loop until we no longer have tokens to parse or we have parsed
  // more tokens then we are capable of storing.
  // todo: the man page says str must be set to null after first run...
  while ((token = strtok_r(strRemainder, delim, &strRemainder)) && tokenIdx < arrSize)
  {
    arr[tokenIdx++] = token;
 
  }

  return 0;
}
