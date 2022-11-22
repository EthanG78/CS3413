#include <stdlib.h>
#include "fat32.h"
#include "Util.h"

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