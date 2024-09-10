#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int varCondivisa = 1;

void *func(void *arg){
	printf("Esecuzione thread figlio \n");
	sleep(2);
	printf("Terminazione thread figlio \n");
	sleep(2);
	varCondivisa = 0;
	pthread_exit(NULL); //terminazione thread figlio
}


int main() {
	int result;
	pthread_t thread;
	void *status;

	printf("Esecuzione thread padre \n");
	sleep(2);
	if(result = pthread_create(&thread, NULL, func, NULL)) {
		printf("Errore \n");
		return 1;
	}
	else {
		pthread_join(thread, &status); //aspetto che termina il figlio
		printf("Terminazione thread padre \n");
		sleep(2);
	}
	return 0;
}
