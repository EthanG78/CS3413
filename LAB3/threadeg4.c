/*  threadeg4.c
    COMP 3430 Operating Systems

    A program to illustrate the effect of multiple threads
    accessing shared data in an uncontrolled fashion.

    To compile: gcc -lpthread threadeg4.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 50
#define NUM_THREADS 20
#define BUSY_WORK 10000

// shared data
int shared = 0;

// mutex variable
pthread_mutex_t mutex;

void *thrfunc(void *dummy)
{
  int i, j, temp, busy;
  double foo = 0;
  pthread_t myID = pthread_self();
  printf("Hi, I am thread <%d>.\n", myID);

  for (i = 0; i < MAX; i++)
  {
    // lock shared resource in critical section
    pthread_mutex_lock(&mutex);
    temp = shared;
    busy = random() % BUSY_WORK;
    for (j = 0; j < busy; j++)
      foo += 3.14159;
    shared = temp + 1;
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);
} // end thrfunc

int main(int argc, char *argv[])
{

  pthread_t threads[NUM_THREADS];
  int rc, t;

  // initialize the random number generator
  srandom(time(NULL));

  pthread_mutex_init(&mutex, NULL)

      for (t = 0; t < NUM_THREADS; t++)
  {
    printf("Creating thread number %d\n", t);
    rc = pthread_create(&threads[t], NULL, thrfunc, NULL);
    if (rc)
    {
      printf("ERROR-return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  } // for

  for (t = 0; t < NUM_THREADS; t++)
  {
    rc = pthread_join(threads[t], NULL);
  } // for

  pthread_mutex_destroy(&mutex);

  printf("\nThe value of the shared variable is: %d\n", shared);
  printf("\nEnd of processing\n");
  pthread_exit(NULL);

} // end main
