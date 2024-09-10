#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main() {
	char *s = "file";
	FILE *file;
	int fd;
	char buffer[100];
	
	fd = open(s, O_RDWR | O_CREAT | O_TRUNC, 0666);
	file = fdopen(fd, "w+");
	
	fprintf(file, "Ciao mondo! \n");
	fflush(file);
	sprintf(buffer, "cat %s \n", s);
	system(buffer);
	return 0;
}

