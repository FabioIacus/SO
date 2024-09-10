#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
	pid_t pid;
	int status;
	char *buffer;
	int i, j;
	size_t len;
	char temp;
	
	printf("Inserisci stringa: ");
	scanf("%m[^\n]", &buffer);
	len = strlen(buffer);
	int n = 2;
	n = n > len ? len : n;
	printf("%s \n", buffer);
	printf("Taglia della stringa: %lu %lu \n", len, sizeof(size_t));

	for (i=0; i<n; i++) {
		pid = fork();
		if (pid == -1) {
			exit(-1);
		}
		if (pid == 0) {
			for (j=0; j<(len/2); j++) {
				temp = buffer[j];
				buffer[j] = buffer[len-j-1];
				buffer[len-j-1] = temp;
			}
			printf("Stringa rovesciata: %s \n", buffer);
			fflush(stdout);
			exit(0);
		}
		else {
			wait(&status);
		}
	}

	printf("\n");
	free(buffer);
}	
