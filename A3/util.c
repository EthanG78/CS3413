#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "fat32.h"
#include "util.h"

// FindFirstSectorOfCluster determines the first sector number
// of a given cluster N in the data area of a FAT volume. This
// function must be passed the fat32Head object of the Fat volume.
//
// Returns a uint32_t value that represents the first sector number
// of the cluster N.
uint32_t FindFirstSectorOfCluster(fat32Head *h, uint32_t N)
{
    return ((N - 2) * h->bs->BPB_SecPerClus) + h->firstDataSector;
}

// ReadFat32Entry reads the contents of the FAT table that are
// related to data cluster number N of the FAT volume. This
// function must be passed the fat32Head object of the Fat volume and
// a file descriptor for the FAT volume. This function ONLY WORKS
// FOR FAT32 VOLUMES.
//
// Returns a uint32_t value that represents the contents
// of the read FAT entry. Please note, that if this function
// returns a value >= 0x0FFFFFF8 then that represents EOF
uint32_t ReadFat32Entry(fat32Head *h, uint32_t N)
{
    uint32_t fatOffset = N * 4;
    uint32_t fatSecNum = h->bs->BPB_RsvdSecCnt + (fatOffset / h->bs->BPB_BytesPerSec);
    uint32_t fatEntryOffset = fatOffset % h->bs->BPB_BytesPerSec;

    // go to the specific FAT sector
    off_t offset = lseek(h->fd, (fatSecNum * (uint32_t)h->bs->BPB_BytesPerSec), SEEK_SET);
    if (offset == -1)
    {
        // todo: just return EOF?
        perror("error seeking to fat entry sector");
        return 0x0FFFFFF8;
    }

    // todo: do we need to read the ENTIRE sector
    // into memory? can we just read the 4 bytes we need?
    // read sector into sector byte array
    uint8_t secBuff[h->bs->BPB_BytesPerSec];
    int bytesRead = read(h->fd, secBuff, h->bs->BPB_BytesPerSec);
    if (bytesRead == -1)
    {
        perror("error reading fat entry");
        return 0x0FFFFFF8;
    }

    // return the specific entry at calculated offset
    return (*((uint32_t *)&secBuff[fatEntryOffset])) & 0x0FFFFFFF;
}

// ReadCluster reads the byte contents of a given cluster and stores these contents
// in the clusterBuffer of size clusterSize. This function only performs a single read.
//
// ReadCluster returns 1 indicating success, and 0 indicating error.
int ReadCluster(fat32Head *h, uint32_t cluster, uint8_t *clusterBuffer, uint32_t clusterSize)
{
    uint32_t firstSectorofCluster = FindFirstSectorOfCluster(h, cluster);
    off_t offset = lseek(h->fd, firstSectorofCluster * (uint32_t)h->bs->BPB_BytesPerSec, SEEK_SET);
    if (offset == -1)
    {
        perror("error seeking to cluster");
        return 0;
    }

    int bytesRead = read(h->fd, clusterBuffer, clusterSize);
    if (bytesRead == -1)
    {
        perror("error reading cluster bytes");
        return 0;
    }

    return 1;
}