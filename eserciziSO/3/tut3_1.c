#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int varCondivisa = 1;

void *func(void *arg) {
	printf("2. Esecuzione del thread figlio... \n");
	sleep(3);
	printf("3. Terminazione del thread figlio... \n");
	sleep(3);
	varCondivisa = 0;
	pthread_exit(NULL); //terminazione thread figlio
}

int main(){
	int result;
	pthread_t thread;
	
	printf("1. Esecuzione del thread padre... \n");
	sleep(3);
	if(result = pthread_create(&thread, NULL, func, NULL)) { 
		//create ritorna un valore diverso da 0 se fallisce
		//quindi se ritorna un valore diverso da 0 è True
		//se fallisce la creazione del thread entra nell'if
		printf("Non è stato possibile creare il thread \n");
		return 1;
	} else {
		while(varCondivisa);
		printf("4. Terminazione del thread padre... \n");
		sleep(3);
	}
	return 0;	
}
