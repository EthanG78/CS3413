/*
  mutex_recursive.c
  COMP 3430 Operating Systems

  Testing recursive mutex. Create a recursive mutex lock, then
  try locking it multiple times, and unlocking it the same number
  of times.

  to compile: gcc -lpthread mutex_recursive.c
*/

// The following #define is needed in Linux so it
// will recognize the symbolic constants PTHREAD_MUTEX_*
#define _GNU_SOURCE

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>

pthread_mutexattr_t attributes;
pthread_mutex_t mutex;

void test_recursive_mutex(){
  int i, j;
  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex, &attributes);
  printf("Testing recursive mutex.\n");
  for(i = 0; i < 10; i++){
    for(j = 0; j < i; j++)
      printf("%c", ' ');
    printf("locking\n");
    pthread_mutex_lock(&mutex);
  }// for
  for(i = 9; i >= 0; i--){
    for(j = 0; j < i; j++)
      printf("%c", ' ');
    printf("unlocking\n");
    pthread_mutex_unlock(&mutex);
  }// for
}// test_recursive_mutex


int main(){
  test_recursive_mutex();
  printf("\nEnd of processing\n");
  return 0;
}// main
