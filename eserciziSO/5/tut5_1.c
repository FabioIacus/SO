#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
//prende come parametri un numero N
//e una stringa S da usare come nome del file da creare
//scrive N byte del file S su un buffer in output
#define max_buf 1024

int main(int argc, char **argv) {
	if(argc != 3) return 1;
	char *input = malloc(max_buf);
	char *output = malloc(max_buf);
	int N = atoi(argv[1]);
	printf("Numero di byte da leggere: %d \n",N);
	char *s = argv[2];
	printf("Inserisci contenuto del file: ");
	scanf("%[^\n]", input);
	printf("Numero di byte totali del file: %ld \n", strlen(input));
	
	int des;
	des = open(s, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(des == -1) return 1;
	write(des, input, strlen(input)); //scrittura su file da input
	lseek(des, 0, 0); //riposizionamento indice
	read(des, output, N); //scrittura su buffer output da file
	printf("Stringa riversata nel buffer: %s \n", output);
	free(input);
	input = NULL;
	free(output);
	output = NULL;
	close(des);
	return 0;
}


