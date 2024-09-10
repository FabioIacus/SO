/*
	1)ricevere da argv[] i pathname associati a N file (N >= 1)
	2)per ogni file generare un thread
	3)il main acquisisce stringhe da stdin in un ciclo indefinito
	4)ogni thread scrive ogni stringa acquisita dal main nel file ad esso associato
	5)gestire SIGINT in modo che quando uno dei thread viene colpito
	  deve stampare a terminale tutte le stringhe già memorizzate nel file destinazione
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <pthread.h>
//#include <sys/mman.h> non serve
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define SIZE 128

long i;
int c;
int sd, sd1;
//struct sembuf oper; se la dichiaro globale dopo il ^C va in semop
char buffer[SIZE];
char **filenames;
__thread char* pathname; //variabile di cui esisterà a tempo di esecuzione una istanza per ogni thread attivato

void printer(int dummy) {
	//stampare a terminale tutte le stringhe già memorizzate nel file
	char buffer[SIZE];
	int ret;
	
	ret = sprintf(buffer, "cat %s", pathname);
	if (ret == -1) {
		printf("Errore stampa del file \n");
		return;
	}
	system(buffer);
	return;
}

void *thread_func(void *arg) {
	int fd;
	long me;
	int ret;
	struct sembuf oper;
	
	me = (long) arg-1; //il primo sarà 0
	
	printf("L'indice è: %ld \n", me);
	
	pathname = (char*) filenames[me+1];
	
	fd = open(pathname, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd == -1) {
		printf("Errore apertura file \n");
		exit(-1);
	}
	printf("File %s correttamente aperto \n",pathname);
	
	//devo prendere un gettone da sd1
	//effettuare l'operazione
	//aggiungere un gettone a sd
	while(1) {
		oper.sem_num = me;
		oper.sem_flg = 0;
		oper.sem_op = -1;
redo1:
		ret = semop(sd1, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (1) \n");
			exit(1);
		}
redo2:
		ret = write(fd, buffer, strlen(buffer)+1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore write \n");
			exit(1);
		}
		oper.sem_num = 0;
		oper.sem_op = 1;
		oper.sem_flg = 0;
redo3:
		ret = semop(sd, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (2) \n");
			exit(1);
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	int fd;
	int ret;
	int n;
	c = argc;
	struct sembuf oper;
	pthread_t tid;
	
	if (argc < 2) {
		printf("Numero argomenti insufficiente \n");
		return -1;
	}
	
	n = argc-1;
	pathname = argv[1];
	filenames = argv;
	
	for (i=1; i<c; i++) {
		fd = open(argv[i], O_RDONLY);
		if (fd == -1) {
			printf("File non esistente \n");
			return -1;
		}
	}
	
	sd = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (sd == -1) {
		printf("Errore semget sd\n");
		return -1;
	}
	
	sd1 = semget(IPC_PRIVATE, n, IPC_CREAT | IPC_EXCL | 0666);
	if (sd1 == -1){
		printf("Errore semget sd1 \n");
		return -1;
	}
	
	ret = semctl(sd, 0, SETVAL, n);
	if (ret == -1) {
		printf("Errore semctl \n");
		return -1;
	}
	
	for (i=0; i<n; i++) {
		ret = semctl(sd1, i, SETVAL, 0);
		if (ret == -1) {
			printf("Errore semctl \n");
			return -1;	
		}
	}
	
	for (i=0; i<n; i++) {
		ret = pthread_create(&tid, NULL, thread_func, (void *) i+1);
		if (ret == -1) {
			printf("Errore pthread_create \n");
			return -1;
		}
	}
	
	signal(SIGINT, printer);
	
	//il main prende n gettoni da sd
	//scrive una stringa su buffer
	//aggiunge un gettone all'i-esimo elemento di sd1 per avvisare della scrittura
	while (1) {
		oper.sem_num = 0;
		oper.sem_op = -n;
		oper.sem_flg = 0;
		
redo1:
		ret = semop(sd, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (4) \n");
			exit(1);
		}

redo2:
		ret = scanf("%s", buffer);
		if (ret == EOF && errno == EINTR) goto redo2;
		if (ret == EOF && errno != EINTR) {
			printf("Errore scanf \n");
			exit(1);
		}
		oper.sem_op = 1;
		oper.sem_flg = 0;
		for (i=0; i<n; i++) {
			oper.sem_num = i;
redo3:
			ret = semop(sd1, &oper, 1);
			if (ret == -1 && errno == EINTR) goto redo3;
			if (ret == -1 && errno != EINTR) {
				printf("Errore semop (3) \n");
				exit(1);
			}
		}

	}
	
	return 0;
}