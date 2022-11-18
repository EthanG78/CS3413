#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "fat32.h"

fat32Head *createHead(int fd)
{
    // malloc new fat32BS struct
    fat32BS *bootSector = (fat32BS *)malloc(sizeof(fat32BS));

    // read the boot sector and bpb from top of fat32 file
    int nBytesRead = read(fd, bootSector, sizeof(fat32BS));
    if (nBytesRead == -1)
    {
        perror("error reading fat32 boot sector");
        return NULL;
    }

    printf("%d", bootSector->BPB_NumFATs);

    free(bootSector);

    return NULL;
}

int cleanupHead(fat32Head *h)
{
    return 0;
}