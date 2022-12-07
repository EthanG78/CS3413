/*
* fat32.h
* CS3413 Assignment 3
* Supplied for assignment with 
* modifications made by Ethan Garnier
*/
#ifndef FAT32_H
#define FAT32_H

#include <inttypes.h>

/* boot sector constants */
#define BS_OEMName_LENGTH 8
#define BS_VolLab_LENGTH 11
#define BS_FilSysType_LENGTH 8
#define DIR_Name_LENGTH 11
#define EOC 0x0FFFFFFF

#pragma pack(push)
#pragma pack(1)
struct fat32BS_struct
{
	char BS_jmpBoot[3];
	char BS_OEMName[BS_OEMName_LENGTH];
	uint16_t BPB_BytesPerSec;
	uint8_t BPB_SecPerClus;
	uint16_t BPB_RsvdSecCnt;
	uint8_t BPB_NumFATs;
	uint16_t BPB_RootEntCnt;
	uint16_t BPB_TotSec16;
	uint8_t BPB_Media;
	uint16_t BPB_FATSz16;
	uint16_t BPB_SecPerTrk;
	uint16_t BPB_NumHeads;
	uint32_t BPB_HiddSec;
	uint32_t BPB_TotSec32;
	uint32_t BPB_FATSz32;
	uint16_t BPB_ExtFlags;
	uint8_t BPB_FSVerLow;
	uint8_t BPB_FSVerHigh;
	uint32_t BPB_RootClus;
	uint16_t BPB_FSInfo;
	uint16_t BPB_BkBootSec;
	char BPB_reserved[12];
	uint8_t BS_DrvNum;
	uint8_t BS_Reserved1;
	uint8_t BS_BootSig;
	uint32_t BS_VolID;
	char BS_VolLab[BS_VolLab_LENGTH];
	char BS_FilSysType[BS_FilSysType_LENGTH];
	char BS_CodeReserved[420];
	uint8_t BS_SigA;
	uint8_t BS_SigB;
};
#pragma pack(pop)
typedef struct fat32BS_struct fat32BS;

#pragma pack(push)
#pragma pack(1)
struct fat32FSInfo_struct
{
	uint32_t FSI_LeadSig;
	char FSI_Reserved1[480];
	uint32_t FSI_StrucSig;
	uint32_t FSI_Free_Count;
	uint32_t FSI_Nxt_Free;
	char FSI_Reserved2[12];
	uint32_t FSI_TrailSig;
};
#pragma pack(pop)
typedef struct fat32FSInfo_struct fat32FSInfo;

// directory attributes
#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID

#pragma pack(push)
#pragma pack(1)
struct fat32Dir_struct
{
	char DIR_Name[DIR_Name_LENGTH];
	char DIR_Attr;
	char DIR_NTRes;
	char DIR_CrtTimeTenth;
	char DIR_CrtTime[2];
	char DIR_CrtDate[2];
	char DIR_LstAccDate[2];
	char DIR_FstClusHI[2];
	char DIR_WrtTime[2];
	char DIR_WrtDate[2];
	char DIR_FstClusLO[2];
	uint32_t DIR_FileSize;
};
#pragma pack(pop)
typedef struct fat32Dir_struct fat32Dir;

#pragma pack(push)
#pragma pack(1)
struct fat32Head
{
	int fd;
	char *volumeID;
	fat32BS *bs;
	fat32FSInfo *fsInfo;
	uint32_t firstDataSector;
	uint32_t nClusters;
};
#pragma pack(pop)
typedef struct fat32Head fat32Head;

fat32Head *createHead(int fd);

fat32FSInfo *readFSInfo(int fd, fat32BS *bs);

int cleanupHead(fat32Head *h);

#endif
