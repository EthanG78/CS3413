uint32_t FindFirstSectorOfCluster(fat32Head *h, uint32_t N);
uint32_t ReadFat32Entry(int fd, fat32Head *h, uint32_t N);