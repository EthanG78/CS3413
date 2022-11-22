#ifndef FAT32_H
#define FAT32_H

#include <inttypes.h>

/* boot sector constants */
#define BS_OEMName_LENGTH 8
#define BS_VolLab_LENGTH 11
#define BS_FilSysType_LENGTH 8
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

#pragma pack(push)
#pragma pack(1)
struct fat32Dir_struct
{
	char DIR_Name[11];
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

// TODO: YOU WILL NEED TO MAKE MORE STRUCTS
// for each struct you make, it's IMPORTANT to surround them
// with the #pragmas you see above like:
#pragma pack(push)
#pragma pack(1)
struct fat32Head
{
	// TODO
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
