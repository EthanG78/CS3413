#define _FILE_OFFSET_BITS 64

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "fat32.h"
#include "util.h"

fat32Head *createHead(int fd)
{
    // malloc new fat32BS struct
    fat32BS *bootSector = (fat32BS *)malloc(sizeof(fat32BS));

    // read the boot sector and bpb from top of fat32 file
    int nBytesRead = read(fd, bootSector, sizeof(fat32BS));
    if (nBytesRead == -1)
    {
        perror("error reading fat32 boot sector");
        free(bootSector);
        return NULL;
    }

    // check signature bytes
    if (bootSector->BS_BootSig != 0x29 || bootSector->BS_SigA != 0x55 || bootSector->BS_SigB != 0xAA)
    {
        free(bootSector);
        return NULL;
    }

    // malloc new fat32head struct
    fat32Head *head = (fat32Head *)malloc(sizeof(fat32Head));

    // set boot sector of fat32Head
    head->bs = bootSector;

    // determine the number of sectors in the root directory region
    // note: on FAT32 this is always 0
    uint32_t rootDirSectors = ((head->bs->BPB_RootEntCnt * 32) + (head->bs->BPB_BytesPerSec - 1)) / head->bs->BPB_BytesPerSec;

    // set first data sector of volume
    uint32_t fatSz = (head->bs->BPB_FATSz16 == 0) ? head->bs->BPB_FATSz32 : (uint32_t)head->bs->BPB_FATSz16;
    head->firstDataSector = head->bs->BPB_RsvdSecCnt + (head->bs->BPB_NumFATs * fatSz) + rootDirSectors;

    // determine the number of clusters
    uint32_t nSectors = (head->bs->BPB_TotSec16 == 0) ? head->bs->BPB_TotSec32 : (uint32_t)head->bs->BPB_TotSec16;
    uint32_t nDataSectors = nSectors - (head->bs->BPB_RsvdSecCnt + (head->bs->BPB_NumFATs * fatSz) + rootDirSectors);
    head->nClusters = nDataSectors / head->bs->BPB_SecPerClus;

    // ensure that the number of clusters is within the fat32 range
    // if there are less than 65525 data clusters, than we are reading
    // either a FAT12 or FAT16 volume and we should return NULL.
    if (head->nClusters < 65525)
    {
        cleanupHead(head);
        return NULL;
    }

    // navigate to start of FAT
    off_t offset = lseek(fd, (head->bs->BPB_RsvdSecCnt * head->bs->BPB_BytesPerSec), SEEK_SET);
    if (offset == -1)
    {
        perror("error seeking to start of fat");
        cleanupHead(head);
        return NULL;
    }

    // read the first two FAT entries (signatures)
    // and ensure their signatures are correct
    uint32_t fatSig0 = ReadFat32Entry(fd, head, 0);
    uint32_t fatSig1 = ReadFat32Entry(fd, head, 1);
    
    uint32_t sig1 = 0x0FFFFF00 | (uint32_t)head->bs->BPB_Media;
    if (fatSig0 != sig1 || fatSig1 != EOC)
    {
        cleanupHead(head);
        return NULL;
    }

    return head;
}

int cleanupHead(fat32Head *h)
{
    if (h != NULL)
    {
        free(h->bs);
        free(h);
    }

    return 0;
}