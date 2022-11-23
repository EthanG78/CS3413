#ifndef UTIL_H
#define UTIL_H

uint32_t FindFirstSectorOfCluster(fat32Head *h, uint32_t N);

uint32_t ReadFat32Entry(fat32Head *h, uint32_t N);

int ReadCluster(fat32Head *h, uint32_t cluster, uint8_t *clusterBuffer, uint32_t clusterSize);

#endif