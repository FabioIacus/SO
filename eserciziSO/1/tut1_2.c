#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	int size = 0;
	char buffer[1024];
	int i,j = 0;
	char temp;
	printf("Il programma ha %d parametri. \n", argc);
	for (j=1; j<=argc; j++){
		printf("Parametro %d: %s \n", j, argv[j-1]);
	}
	size = strlen(argv[1]); //lunghezza di argv[1]
	printf("lunghezza param1: %d \n", size);
	//copia carattere per carattere
	for (i=0; i<=size; i++) {
		buffer[i] = argv[1][i]; //accedo all'iesimo char di argv[1]
	}
	printf("%d \n", i);
	printf("Stringa copiata nel buffer: %s \n", buffer);
	printf("La seconda lettera di argv[1] è: %c \n", argv[1][1]);
	printf("La prima lettere di argv[2] è: %c \n", argv[2][0]);
	for (j=0; j < i/2; j++) {
		temp = buffer[j];
		buffer[j] = buffer[size-j-1];
		buffer[size-j-1] = temp;
	}
	printf("Buffer rigirato: %s \n", buffer);
	return 0;
}
