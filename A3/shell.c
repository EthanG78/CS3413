#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "shell.h"
#include "util.h"

#define BUF_SIZE 256
#define CMD_INFO "INFO"
#define CMD_DIR "DIR"
#define CMD_CD "CD"
#define CMD_GET "GET"
#define CMD_PUT "PUT"
#define CMD_EXIT "EXIT"

void shellLoop(int fd)
{
	int running = true;
	uint32_t curDirClus;
	char buffer[BUF_SIZE];
	char bufferRaw[BUF_SIZE];

	// TODO:
	fat32Head *h = createHead(fd);

	if (h == NULL)
		running = false;
	else								  // valid, grab the root cluster
		curDirClus = h->bs->BPB_RootClus; // TODO

	while (running)
	{
		printf(">");
		if (fgets(bufferRaw, BUF_SIZE, stdin) == NULL)
		{
			running = false;
			continue;
		}
		bufferRaw[strlen(bufferRaw) - 1] = '\0'; /* cut new line */
		for (int i = 0; i < strlen(bufferRaw) + 1; i++)
			buffer[i] = toupper(bufferRaw[i]);

		if (strncmp(buffer, CMD_INFO, strlen(CMD_INFO)) == 0)
			printInfo(h);

		else if (strncmp(buffer, CMD_DIR, strlen(CMD_DIR)) == 0)
			doDir(h, curDirClus);

		else if (strncmp(buffer, CMD_CD, strlen(CMD_CD)) == 0)
			curDirClus = doCD(h, curDirClus, buffer);

		else if (strncmp(buffer, CMD_GET, strlen(CMD_GET)) == 0)
			doDownload(h, curDirClus, buffer);

		else if (strncmp(buffer, CMD_PUT, strlen(CMD_PUT)) == 0)
			// doUpload(h, curDirClus, buffer, bufferRaw);
			printf("Bonus marks!\n");
		else if (strncmp(buffer, CMD_EXIT, strlen(CMD_EXIT)) == 0)
			running = false;
		else
			printf("\nCommand not found\n");
	}
	printf("\nExited...\n");

	cleanupHead(h);
}

int printInfo(fat32Head *h)
{
	/*
		Need to print 3 sections:
		1. Device Info:
			- OEM Name
			- Label
			- File System Type
			- Media Type
			- Size
			- Drive Number
	*/
	printf("---- Device Info ----\n");

	char oemname[BS_OEMName_LENGTH + 1];
	snprintf(oemname, BS_OEMName_LENGTH + 1, "%s", h->bs->BS_OEMName);
	printf(" OEM Name: %s\n", oemname);

	char vollab[BS_VolLab_LENGTH + 1];
	snprintf(vollab, BS_VolLab_LENGTH + 1, "%s", h->bs->BS_VolLab);
	printf(" Label: %s\n", vollab);

	char filsystype[BS_FilSysType_LENGTH + 1];
	snprintf(filsystype, BS_FilSysType_LENGTH + 1, "%s", h->bs->BS_FilSysType);
	printf(" File System Type: %s\n", filsystype);

	if ((h->bs->BPB_Media & 0xF8) == 0xF8)
	{
		printf(" Media Type: 0x%X (fixed)\n", h->bs->BPB_Media);
	}
	else
	{
		printf(" Media Type: 0x%X (not fixed)\n", h->bs->BPB_Media);
	}

	uint32_t totalSectors = (h->bs->BPB_TotSec16 == 0)
								? h->bs->BPB_TotSec32
								: h->bs->BPB_TotSec16;

	uint64_t totalBytes = (uint64_t)h->bs->BPB_BytesPerSec * (uint64_t)totalSectors;
	double totalMBytes = (double)totalBytes / (1000000);
	double totalGBytes = (double)totalBytes / (1000000000);

	printf(" Size: %ld (%.3fMB, %.3fGB)\n", totalBytes, totalMBytes, totalGBytes);

	printf(" Drive Number: %d ", h->bs->BS_DrvNum);

	if ((h->bs->BS_DrvNum & 0x80) == 0x80)
	{
		printf("(hard drive)\n");
	}
	else if ((h->bs->BS_DrvNum & 0x00) == 0x00)
	{
		printf("(floppy disk)\n");
	}
	else
	{
		printf("(unknown)\n");
	}

	/*
		2. Geometry
			- Bytes per Sector
			- Sectors per Cluster
			- Total Sectors
			- Geom: Sectors per Track
			- Geom: Heads
			- Hidden Sectors
	*/
	printf("\n---- Geometry ----\n");
	printf(" Bytes per Sector: %d\n", h->bs->BPB_BytesPerSec);
	printf(" Sectors per Cluster: %d\n", h->bs->BPB_SecPerClus);
	printf(" Total Sectors: %d\n", totalSectors);
	printf(" Geom: Sectors per Track: %d\n", h->bs->BPB_SecPerTrk);
	printf(" Geom: Heads: %d\n", h->bs->BPB_NumHeads);
	printf(" Hidden Sectors: %d\n", h->bs->BPB_HiddSec);

	/*
		3. FS Info
			- Volume ID
			- Version
			- Reserved Sectors
			- Number of FATs
			- FAT Size
			- Mirrored FAT
			- Boot Sector Backup Sector No
	*/
	printf("\n---- FS Info ----\n");
	printf(" Volume ID: TODO!!!!\n");
	printf(" Version: %d.%d\n", h->bs->BPB_FSVerHigh, h->bs->BPB_FSVerLow);
	printf(" Reserved Sectors: %d\n", h->bs->BPB_RsvdSecCnt);
	printf(" Number of FATs: %d\n", h->bs->BPB_NumFATs);

	uint32_t fatSize = (h->bs->BPB_FATSz16 == 0)
						   ? h->bs->BPB_FATSz32
						   : h->bs->BPB_FATSz16;
	printf(" FAT Size: %d\n", fatSize);

	uint16_t mirroredFATBit = (h->bs->BPB_ExtFlags & 0x0040) >> 6;
	if (mirroredFATBit)
	{
		printf(" Mirrored FAT: %d (no)\n", mirroredFATBit);
	}
	else
	{
		printf(" Mirrored FAT: %d (yes)\n", mirroredFATBit);
	}

	printf(" Boot Sector Backup Sector No: %d\n", h->bs->BPB_BkBootSec);

	printf("\n");

	return 0;
}

// todo
int doDir(fat32Head *h, uint32_t curDirClus)
{
	printf("\nDIRECTORY LISTING\n");
	printf("VOL_ID: TODO!!!\n\n");

	int success;
	fat32Dir *dir = NULL;

	char name[9];
	char ext[4];

	uint32_t sizeOfCluster = (uint32_t)h->bs->BPB_BytesPerSec * (uint32_t)h->bs->BPB_SecPerClus;
	uint8_t clusterBuff[sizeOfCluster];

	// Keep reading directory entries until we read an EOC in
	// the FAT, this indicates we have reached the last cluster
	// of this particular directory.
	do
	{
		success = ReadCluster(h, curDirClus, clusterBuff, sizeOfCluster);
		if (!success)
		{
			printf("There was an issue reading cluster %d\n", curDirClus);
			return 0;
		}

		// cast the cluster we just read to our dir structure
		dir = (fat32Dir *)(&clusterBuff[0]);

		// keep incrementing dir until we reach the last entry
		while (dir->DIR_Name[0] != 0x00)
		{
			// if dir->DIR_Name[0] == 0xE5
			// then there is nothing stored in
			// it and we may skip it
			if (dir->DIR_Name[0] != 0xE5)
			{
				if ((dir->DIR_Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
				{
					// print directory
					printf("This is a directory: %s\n", dir->DIR_Name);
				}
				else
				{
					if ((dir->DIR_Attr & ATTR_HIDDEN) != ATTR_HIDDEN)
					{
						if (!RemoveTrailingWhiteSpace(dir->DIR_Name, name, 8))
						{
							printf("There was an issue parsing file name: %s\n", dir->DIR_Name);
							return 0;
						}

						if (!RemoveTrailingWhiteSpace(&dir->DIR_Name[8], ext, 3))
						{
							printf("There was an issue parsing file extension: %s\n", dir->DIR_Name);
							return 0;
						}
						printf("File: %s.%s\n", name, ext);
					}
				}
			}

			dir++;
		}

		// read the FAT entry for the current cluster,
		// and store its contents in curDirClus to read next
		curDirClus = ReadFat32Entry(h, curDirClus);
	} while (curDirClus != EOC && dir != NULL);

	uint64_t freeBytes;
	uint32_t freeClus = h->fsInfo->FSI_Free_Count;
	if (freeClus != 0xFFFFFFFF && freeClus <= (h->bs->BPB_TotSec32 * (uint32_t)h->bs->BPB_SecPerClus))
	{
		freeBytes = (uint64_t)freeClus * (uint64_t)h->bs->BPB_SecPerClus * (uint64_t)h->bs->BPB_BytesPerSec;
		printf("---Bytes Free: %ld\n", freeBytes);
	}
	else
	{
		printf("---Bytes Free: unknown\n");
	}

	printf("---DONE\n");

	return 1;
}

int doCD(fat32Head *h, uint32_t curDirClus, char *buffer)
{
	return 0;
}

int doDownload(fat32Head *h, uint32_t curDirClus, char *buffer)
{
	return 0;
}
