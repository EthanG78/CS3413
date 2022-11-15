#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

char name[] = "large.dat";

int main(int argc, char* argv[]) {
	int f;

	char data = (char)0; /* not null! 0s. */
	
	/* create / write the file given */
	f = open(name, O_CREAT | O_WRONLY, 0666);
	write(f, &data, 1); /* write a byte*/
	lseek(f, 100000000, SEEK_SET); /* jump ahead 100mb into the file, will use this middle space later */
	write(f, &data, 1);
	close(f);
	return(0);
}
