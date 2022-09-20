/*
  util.c
  CS3413 Operating Systems 1

  Ethan Garnier
*/

#include <string.h>
#include <util.h>

// tokenizeIntoArr takes a char* stored in str and splits it into tokens
// based on a character delimeter specified by delim. Each of these tokens
// are stored, in order, in the arr char* array. This array may hold arrSize - 1
// tokens. A NULL pointer is placed at the end of the array for compatibility
// with the exec family of functions.
//
// tokenizeIntoArr returns the integer number of tokens stored in arrSize.
//
// Thank you to Dr. Rea for inspiring this helper function.
//
int tokenizeIntoArr(char *str, char **arr, const int arrSize, const char *delim)
{
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
