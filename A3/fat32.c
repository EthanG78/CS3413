#define _FILE_OFFSET_BITS 64

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "fat32.h"

fat32Head *createHead(int fd)
{
    // malloc new fat32head struct
    fat32Head *head = (fat32Head *)malloc(sizeof(fat32Head));

    // malloc new fat32BS struct
    fat32BS *bootSector = (fat32BS *)malloc(sizeof(fat32BS));

    // read the boot sector and bpb from top of fat32 file
    int nBytesRead = read(fd, bootSector, sizeof(fat32BS));
    if (nBytesRead == -1)
    {
        perror("error reading fat32 boot sector");
        return NULL;
    }

    // set boot sector of fat32Head
    head->bs = bootSector;

    // set first data sector of volume
    uint32_t fatSz = (head->bs->BPB_FATSz16 == 0) ? head->bs->BPB_FATSz32 : (uint32_t)head->bs->BPB_FATSz16; 
    head->firstDataSector = head->bs->BPB_RsvdSecCnt + (head->bs->BPB_NumFATs * fatSz);

    return head;
}

int cleanupHead(fat32Head *h)
{
    free(h->bs);
    free(h);

    return 0;
}