/*
  myshell.c
  COMP 3413 Operating Systems 1
  
  Ethan Garnier
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <util.h>

int main(int argc, char *argv[])
{
    char *inputStr;
    int nInput;
    do
    {
      nInput = scanf("%ms", &inputStr);

      if (nInput == 1)
      {
        // successfully read user input
      }
      else if (errno != 0)
      {
        // scanf thrown an error
      }
      else
      {
        
      }
      
    } while (strcmp(inputStr, "exit") != 0);

    // todo:
    // since input is dynamically allocated,
    // i think i need to free it
    free(inputStr);
    
    return 0;
}