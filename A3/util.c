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

// ReadFromCluster reads dataSize amount of bytes starting from cluster startCluster into the
// dataBuff byte buffer that must be of size dataSize. This function only calls the read() function
// once. This function can read multiple contiguous clusters at once, given clusterSize spans multiple
// clusters.
//
// ReadFromCluster returns 1 indicating success, and 0 indicating error.
int ReadFromCluster(fat32Head *h, uint32_t startCluster, uint8_t *dataBuff, uint32_t dataSize)
{
    uint32_t firstSectorofCluster = FindFirstSectorOfCluster(h, startCluster);
    off_t offset = lseek(h->fd, firstSectorofCluster * (uint32_t)h->bs->BPB_BytesPerSec, SEEK_SET);
    if (offset == -1)
    {
        perror("error seeking to cluster");
        return 0;
    }

    int bytesRead = read(h->fd, dataBuff, dataSize);
    printf("bytesRead: %d\n", dataSize);
    if (bytesRead == -1)
    {
        perror("error reading cluster bytes");
        return 0;
    }

    return 1;
}

// RemoveTrailingWhiteSpace replaces all trailing
// white space in str of size size with the null-terminating
// byte. New string is stored in newStr of size size + 1.
// The null terminated byte is appended at the end of newStr
//
// Returns 1 indicating success
int RemoveTrailingWhiteSpace(char *str, char *newStr, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        newStr[i] = (str[i] == ' ') ? '\0' : str[i];
    }

    newStr[size] = '\0';
    return 1;
}

// HasAttribures checks to see if the 32 bit fileAttr field
// contains the attributes outlines by the 8 bit attr field.
//
// Returns 1 indicating that attr is found within fileAttr, 0 otherwise.
int HasAttributes(uint8_t fileAttr, uint8_t attr)
{
    return ((fileAttr & attr) == attr);
}