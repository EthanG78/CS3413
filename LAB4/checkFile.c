#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	struct stat data;
	int result;

	if (argc < 2) {
		printf("usage: \"%s <file>\" to get info on a file\n", argv[0]);
		exit(1);
	}
	
	result = lstat(argv[1], &data);
	if (result == -1) {
		perror("error performing stat on file");
		exit(1);
	}
	printf("file %s has inode #%d\n", argv[1], (int)data.st_ino);
	printf("file %s has %d hard links to its inode\n", argv[1], (int)data.st_nlink);
	return(0);
}
