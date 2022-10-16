/*
mutex-eg1.c
-----------

To compile: gcc -lpthread mutex-eg1.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char *argv[]){

  pthread_mutexattr_t attr;
  int type;
  
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_gettype(&attr, &type);
  printf("mutex type value = %d\n", type);
  switch(type){
  case PTHREAD_MUTEX_NORMAL:
    printf("normal type\n");
    break;
  case PTHREAD_MUTEX_ERRORCHECK:
    printf("error check type\n");
    break;
  case PTHREAD_MUTEX_RECURSIVE:
    printf("recursive type\n");
    break;
  default:
    printf("what have we here\n");
    break;
  }// switch
  
  printf("\nEnd of processing\n");
  return 0;
} /* end main */

