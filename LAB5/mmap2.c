/*
   mmap2.c
   COMP 3413 Operating Systems

   Remake of mmap.c for lab5

   Ethan Garnier

   Supply the file name of file to
   be mmaped through arguments

   to compile: gcc -lrt mmap.c
*/

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h> /* For O_* constants */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 81
char syscmd[80];

int oflag = O_RDONLY;
int fd;
mode_t mode = 0444;
int prot = PROT_READ;
int flags = MAP_SHARED;
void *mapped_file = NULL;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please supply file name through command-line arguments\n");
        exit(0);
    }

    fd = open(argv[1], oflag, mode);
    printf("fd = %d\n", fd);
    if (fd == -1)
    {
        printf("open return error code %d.\n", errno);
        exit(0);
    }

    mapped_file = mmap(0, BUFSIZE, prot, flags, fd, 0);
    if ((long)mapped_file == -1)
    {
        printf("mmap returned error code %d\n", errno);
        exit(0);
    }

    sprintf(syscmd, "pmap -x %d", getpid());
    system(syscmd);

    printf("\nPointer returned by mmap() = %p\n", mapped_file);
    printf("%s\n", (char *)mapped_file);

    close(fd);
    
    munmap(mapped_file, BUFSIZE);

    printf("\nEnd of processing.\n");
    return 0;
} // main
