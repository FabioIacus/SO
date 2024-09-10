#include <stdio.h>

int main(){
	char *buff;
	char str[10];
	printf("Inserisci stringa: ");
	scanf("%ms", &buff);  //scanf(ms) vuole char **
	//scanf("%ms", str);
	//scanf("%s", str);  scanf(s) vuole char *
	printf("buff:'%s' \n", buff);  //printf(s) vuole char *
	//printf("buff:'%s' \n", str);  str mi dà char *
	
	int x;
	int *p = &x;
	printf("Inserisci x: ");
	scanf("%d", p);
	printf("x = %d \n", *p);
	return 0;
}

/* char *x
scanf("%ms", &x)
printf("%s", x) */