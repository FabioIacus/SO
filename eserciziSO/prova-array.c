#include <stdio.h>

int a[3] = {1,2,3};

int main(int argc, char **argv){
	char *p;
	int i;
	printf("%ld \n", sizeof(a)); //stampa numero di byte (4*3)
	for (i=0; i<3; i++){
		printf("%d", a[i]);
	}
	printf("\n");
	p = (char*)a + sizeof(a);
	*p = 69;
	printf("L'ultimo elemento dell'array e': ");
	printf("%d",((int*)p)[-1]); 
	printf("\n");
	printf("valore di p = %d \n", ((int*)p)[0]);
}
