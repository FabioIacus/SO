#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *input(void *arg){
	char *buffer;
	printf("Esecuzione thread figlio 1... \n");
	sleep(2);
	printf("Inserisci stringa: ");
	scanf("%m[^\n]", &buffer);
	printf("Terminazione thread figlio 1... \n");
	sleep(2);
	pthread_exit((void *) buffer);
}

void *output(void *arg){
	char *buffer;
	buffer = (char *) arg;
	printf("Esecuzione thread figlio 2... \n");
	sleep(2);
	if (buffer == NULL) {
	       	printf("Errore nell'acquisizione della stringa");
		pthread_exit((void *) 1);
	}
	printf("Stringa presa in input: %s \n", buffer);
	printf("Terminazione thread figlio 2... \n");
	sleep(2);
	pthread_exit((void *) 0);
}

int main() {
	pthread_t t;
	void *exit_status;
	int result;
	void *buffer;
	
	printf("Esecuzione thread padre... \n");
	sleep(2);

	if (result = pthread_create(&t, NULL, input, NULL)) {
		printf("Errore \n");
		return 1;
	}
	
	if (pthread_join(t, &buffer)){
		printf("Errore nella terminazione del thread \n");
		return 1;
	} 

	if (result = pthread_create(&t, NULL, output, buffer)) {
		printf("Errore \n");
		return 1;
	}

	if (pthread_join(t, &exit_status)){
		printf("Errore nella terminazione del thread \n");
		return 1;
	}

	free(buffer);

	return 0;
}
