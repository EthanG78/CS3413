#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
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

// WIP:
// ReadFat32Entry reads the contents of the FAT table that are
// related to data cluster number N of the FAT volume. This
// function must be passed the fat32Head object of the Fat volume and
// a file descriptor for the FAT volume. This function ONLY WORKS 
// FOR FAT32 VOLUMES.
//
// Returns a uint32_t value that represents the contents
// of the read FAT entry. Please note, that if this function
// returns a value >= 0x0FFFFFF8 then that represents EOF
uint32_t ReadFat32Entry(int fd, fat32Head *h, uint32_t N)
{
    uint32_t fatOffset = N * 4;
    uint32_t fatSecNum = h->bs->BPB_RsvdSecCnt + (fatOffset / h->bs->BPB_BytesPerSec);
    uint32_t fatEntryOffset = fatOffset % h->bs->BPB_BytesPerSec;

    // go to the specific FAT sector
    off_t offset = lseek(fd, (fatSecNum * (uint32_t)h->bs->BPB_BytesPerSec), SEEK_SET);
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
    int bytesRead = read(fd, secBuff, h->bs->BPB_BytesPerSec);

    // return the specific entry at calculated offset
    return (*((uint32_t *) &secBuff[fatEntryOffset])) & 0x0FFFFFFF;
}