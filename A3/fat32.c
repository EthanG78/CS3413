#include <unistd.h>
#include <stdlib.h>
#include "fat32.h"

fat32Head* createHead(int fd)
{
    fat32BS *bootSector;
    int nBytesRead = read(fd, bootSector, sizeof(fat32BS));
    if (nBytesRead == -1)
    {
        perror("error reading fat32 boot sector");
        return NULL;
    }
    
    printf("%d", bootSector->BPB_BytesPerSec);

    return NULL;
}

int cleanupHead(fat32Head *h)
{
    return 0;
}