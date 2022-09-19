/*
	myshell.c
	COMP 3413 Operating Systems 1
	
	Modified from lab2 handout.
	
	To compile: gcc myshell.c
	
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define BUFSIZE 81

int main(int argc, char *argv[]){

  int ret_code;		// return code
  int len;		// length of entered command
  int pid;		// process pid
  char buffer[BUFSIZE];	// room for 80 chars plus \0
  char *cmd;		// pointer to entered command

  // Print a prompt and read a command from standard input
  printf("Enter a command: > ");
  cmd = fgets(buffer, BUFSIZE, stdin);
  
  // did the user enter a command?
  while(cmd != NULL){
    // check for the newline character and overwrite with \0
    len = strlen(buffer);
    if(buffer[len-1] == '\n'){
      buffer[len-1] = '\0';
    }

    pid = fork();
    if (pid != 0)
    {
      // parent process
      waitpid(pid, NULL, 0);
    }
    else
    {
      // execute the command
      ret_code = execlp(cmd, cmd, NULL);
      if(ret_code != 0){
        printf("Error executing %s.\n", cmd);
        exit(0);
      }
    }
    printf("Enter a command: > ");
    cmd = fgets(buffer, BUFSIZE, stdin);
  }
  printf("All done.\n");
  return 0;
} // main
