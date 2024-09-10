#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//il processo principale genera N thread
//ogni thread con indice i prima stampa il messaggio ricevuto dal thread con indice i-1
//e poi chiede all'utente di inserire un messaggio per il thread i+1
//le operazioni di lettura, stampa e richiesta devono essere sequenzializzate
int n = 3;
void *buffer;

void *func(void *arg){
	long int id = (long int) arg;
	printf("Thread numero %ld \n", id);
	if(id > 0){
		printf("Messaggio precedente: %s \n", (char *) buffer);
	}
	if(id < n-1) {
		printf("Inserisci messaggio per il prossimo thread: ");
		scanf("%m[^\n]", (char **) &buffer);
	}
	pthread_exit(buffer);
}

int main(){
	pthread_t t;
	int result;
	void *exit_status;
	
	for(int i=0; i<n; i++) {
		if(result = pthread_create(&t, NULL, func, (void *) ((long)i))){
			printf("Errore creazione thread \n");
			return 1;
		}
		pthread_join(t, &buffer);
	}
	pthread_join(t, &exit_status);
	/*for(int i=0; i<n; i++){
		if(result = pthread_join(t[i], &exit)){
			printf("Errore terminazione thread \n");
			return 1;
		}
	}
	//printf("Buffer: %s \n", (char *) buffer);
	printf("Terminazione processo padre \n");*/
	return 0;
}
