#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int *aux;

void *child_thread(void *p) {
	int c=1;
	aux = &c;
	while(1){
		printf("valore di c: %d \n", c);
		sleep(2);
	}
}

void *interfering_child_thread(void *p) {
	int c;
	while(1){
		scanf("%d", &c);
		*aux = c;
	}
}

int main() {
	pthread_t tid;
	if (pthread_create(&tid, NULL, child_thread, NULL) != 0) {
		printf("errore creazione thread \n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid, NULL, interfering_child_thread, NULL) != 0) {
		printf("errore creazione thread \n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
	pause();
}
