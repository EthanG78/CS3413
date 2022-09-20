/*
  myshell.c
  CS3413 Operating Systems 1
  
  Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <util.h>

int main(int argc, char *argv[])
{
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL)
    {
      // getcwd threw an error,
      // print out the errno error
      perror("getcwd");
    }

    char *inputStr;
    int nInput;

    // this is the main shell loop which will accept
    // user input, handle all errors, and end the 
    // program when the user enters 'exit' 
    do
    {
      // todo:
      // currently if the user enters nothing
      // then cwd is not printed out again...
      printf("%s%%", cwd);
      fflush(stdout);

      // using the dynamic allocation conversion specifier
      // so we do not need to specify buffuer size
      nInput = scanf("%ms", &inputStr);

      if (nInput > 0)
      {
        // successfully read user input
        // printf("You typed: %s\n", inputStr);
        // handle commands
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

    // since cwd is dynamically allocated
    // we must call free on it
    free(cwd);    

    return 0;
}
