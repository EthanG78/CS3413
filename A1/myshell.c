/*
  myshell.c
  COMP 3413 Operating Systems 1
  
  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <util.h>

int main(int argc, char *argv[])
{
    char *inputStr;
    int nInput;

    // this is the main shell loop which will accept
    // user input, handle all errors, and end the 
    // program when the user enters 'exit' 
    do
    {
      // using the dynamic allocation conversion specifier
      // so we do not need to specify buffuer size
      nInput = scanf("%ms", &inputStr);

      if (nInput > 0)
      {
        // successfully read user input
        printf("You typed: %s\n", inputStr);
      }
      else if (errno != 0)
      {
        // scanf threw an error,
        // print out the errno error
        perror("scanf()");
        return 1;
      }
    } while (strcmp(inputStr, "exit") != 0);

    // since input is dynamically allocated
    // we must call free on it
    free(inputStr);
    
    return 0;
}