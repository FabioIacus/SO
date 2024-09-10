#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){
	char *buffer;
	int size = 0;
	scanf("%m[^\n]", &buffer); //allocazione nell'heap da parte della scanf
	printf("Stringa allocata nell'heap: %s \n", buffer);
	while(buffer[size++] != '\0'); //mi trovo la dimensione del buffer
	char str[size];
	int i;
	for (i=0;i<size;i++) {
		str[i] = buffer[i];
	}
	free(buffer);
	buffer = NULL;
	printf("Stringa copiata sullo stack: %s \n", str);
	return 0;
}
