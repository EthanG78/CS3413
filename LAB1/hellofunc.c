#include <stdio.h>
#include <hellomake.h>

#define SUCCESS 0
#define MYNUM 42

int myPrintHelloMake(void)
{
    printf("Hello makefiles! %d\n", MYNUM);
    return SUCCESS;
}