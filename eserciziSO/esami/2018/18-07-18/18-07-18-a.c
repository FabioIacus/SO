/*
	1)Ricevere da argv[] i pathname associati a N file (N >= 1)
	2)Generare N processi
	3)Ogni processo generato legge tutte le stringhe del file e le scrive in un'area di memoria condivisa col padre
	4)Supporre che lo spazio per memorizzare le stringhe di ogni file non ecceda 4KB
	5)Il processo padre attende che tutti i figli abbiano scritto in memoria
	6)Successivamente entrerà in pausa indefinita
	7)Ogni processo figlio dopo aver scritto in memoria tutto il contenuto, entra in pausa indefinita
	8)Gestire il segnale SIGINT: quando il processo padre viene colpito,
	  stampa a terminale il contenuto corrente di tutte le aree di memoria condivisa anche se non sono state
	  completamente popolate dai figli
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
//#include <pthread.h>

#define SIZE 4096

long i;
long N;
int fd;
int sd;
char **mem_seg; 
char *segment;
struct sembuf oper;
int c; //argc

void *child_func() {
	FILE *f;
	
	f = fdopen(fd, "r");
	if (f == NULL) {
		printf("Errore fopen \n");
		return -1;
	}
	
	//leggo dal file e scrivo su segment (l'area di memoria condivisa)
	while (fscanf(f, "%s", segment) != EOF) {
		printf("Letto: %s \n", segment);
		segment += strlen(segment)+1; //mi sposto della lunghezza della stringa letta + 1 per il terminatore
	}
	
	oper.sem_num = 0;
	oper.sem_flg = 0;
	oper.sem_op = 1; //avviso che ho scritto
	
	semop(sd, &oper, 1); //eseguo l'op
	
	while(1) pause();
}

//stampa a terminale il contenuto corrente di tutte le aree di memoria condivisa
//anche se non sono state completamente popolate dai figli
/*void handler(int signo, siginfo_t *a, void *b) {
	int i;
	//segment; non necessario, non si sa a cosa serva
	
	for (i=1; i<c; i++) {
		segment = mem_seg[i];
		while (strcmp(segment, "\0") != 0) {
			printf("%s \n", segment);
			//printf("%s \n", mem_seg[i]);
			segment += strlen(segment)+1;
		}
	}
}*/
void handler(int dummy) {
	int i;
	for (i=1; i<c; i++) {
		segment = mem_seg[i];
		while (strcmp(segment, "\0") != 0) {
			printf("%s \n", segment);
			//printf("%s \n", mem_seg[i]);
			segment += strlen(segment)+1;
		}
	}
}


int main(int argc, char *argv[]) {
	int ret;
	//sigset_t set;
	//struct sigaction act; inutili se faccio la signal
	
	c = argc;
	
	if (argc < 2) {
		printf("Numero argomenti insufficiente \n");
		return -1;
	}
	
	//se dice pathname devo usare open e file descriptor
	//controllo l'esistenza dei file (non necessario)
	for (i=1; i<argc; i++) {
		fd = open(argv[i], O_RDONLY);
		if (fd == -1) {
			printf("File %s non esistente \n", argv[i]);
		}
	}
	
	//allocazione memoria condivisa
	mem_seg = malloc(sizeof(char*)*argc);
	if (mem_seg == NULL) {
		printf("Errore allocazione memoria condivisa \n");
		return -1;
	}
	
	//creazione semaforo
	sd = semget(IPC_PRIVATE, 1, 0660);
	if (sd == -1) {
		printf("Errore semget \n");
		return -1;
	}
	
	//settaggio semaforo a 0
	ret = semctl(sd, 0, SETVAL, 0);
	if (ret == -1) {
		printf("Errore settaggio semaforo \n");
		return -1;
	}
	
	//mappaggio memoria e creazione N processi
	for (i=1; i<argc; i++) {
		mem_seg[i] = (char*) mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
		if (mem_seg[i] == NULL) {
			printf("Errore mmap \n");
			return -1;
		}
		segment = mem_seg[i];
		
		fd = open(argv[i], O_RDONLY);
		
		ret = fork();
		if (ret == -1) {
			printf("Errore fork \n");
			return -1;
		}
		if (ret == 0) {
			signal(SIGINT, SIG_IGN); //il processo figlio ignora il segnale SIGINT
			child_func();
		}
	}
	
	//gestione del segnale SIGINT
	/*sigfillset(&set); //inserisce tutti i segnali in set
	
	act.sa_sigaction = handler;
	act.sa_mask = set;
	act.sa_flags = 0;
	
	sigaction(SIGINT, &act, NULL);*/ //permette di modificare la gestione del segnale SIGINT
	signal(SIGINT, handler); 
	
	
	//il padre attende che tutti i figli abbiano finito di scrivere in memoria
	oper.sem_num = 0; //il primo elemento
	oper.sem_flg = 0;
	oper.sem_op = -(argc-1); //prendo N gettoni dove N è il numero di processi figli
	
redo:
	ret = semop(sd, &oper, 1); //eseguo l'operazione specificata in oper
	if (ret == -1 && errno == EINTR) goto redo;
	if (ret == -1 && errno != EINTR) {
		printf("Errore semop \n");
		return -1;
	}
	printf("Tutti i child hanno rilasciato il gettone \n");
	while(1) pause(); //rimane in attesa indefinita
		
	return 0;
}
