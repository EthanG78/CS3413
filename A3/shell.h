/*
* shell.h
* CS3413 Assignment 3
* Author: Ethan Garnier
*/
#ifndef SHELL_H
#define SHELL_H

#include "fat32.h"

void shellLoop(int fd);

int printInfo(fat32Head *h);

int doDir(fat32Head *h, uint32_t curDirClus);

uint32_t doCD(fat32Head *h, uint32_t curDirClus, char *buffer);

int doDownload(fat32Head *h, uint32_t curDirClus, char *buffer);

#endif
