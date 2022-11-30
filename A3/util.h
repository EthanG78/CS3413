#ifndef UTIL_H
#define UTIL_H

uint32_t FindFirstSectorOfCluster(fat32Head *h, uint32_t N);

uint32_t ReadFat32Entry(fat32Head *h, uint32_t N);

int ReadFromCluster(fat32Head *h, uint32_t startCluster, uint8_t *dataBuff, uint32_t dataSize);

int RemoveTrailingWhiteSpace(char* str, char *newStr, int size);

int HasAttributes(uint8_t fileAttr, uint8_t attr);

#endif