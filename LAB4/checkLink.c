#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DATASIZE 10000

int main(int argc, char* argv[]) {
    char data[DATASIZE + 1];
	int result;

    // Error checking on arguments
	if (argc < 2) {
		printf("usage: \"%s <link>\" to get info on a link\n", argv[0]);
		exit(1);
	}

    // Call readlink on passed link file and store in data variable
    result = readlink(argv[1], data, DATASIZE);
    if (result == -1) {
		perror("error calling readlink()");
		exit(1);
	}

    // Append null byte
    data = data + '\0';

    // Print raw link contents
    printf("link %s has contents:\n%s", argv[1], data);

	return(0);
}
