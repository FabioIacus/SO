/*  1)ricevere da argv una stringa e un numero N >= 1
	2)creare file F con nome la stringa ricevuta in input
	3)attivare N thread
	4)attivare un processo figlio che attiverà N thread
	5)il padre verrà chiamato A e il figlio B
	6)ciascun thread di A leggerà stringhe da stdin
	7)ogni stringa letta viene comunicata al corrispettivo thread di B tramite memoria condivisa
	8)questo la scriverà su una nuova linea del file F (assumere che ogni stringa non eccede la taglia di 4kb)
	9)gestire il segnale SIGINT in modo tale che:
	9.1)se il processo A viene colpito dovrà inviare lo stesso segnale a B
	9.2)se viene colpito B, questo dovrà riversare su stdout il contenuto corrente di F
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>

#define SIZE 4096

long N; //numero thread
FILE *file;
int fd;
char *name; //nome file
pid_t pid;
char **mem_seg;  //memoria condivisa
int sd1, sd2; //descrittori semafori

void parent_handler(int signo) {
	printf("Parent received signal %d - forwarding to child (pid %d) \n", signo, pid);
	kill(pid, signo); //invio segnale al processo figlio
}

void child_handler(int dummy) {
	char buffer[1024];
	sprintf(buffer, "cat %s \n", name); //scrive su buffer il comando da eseguire
	system(buffer); //systemcall per eseguire un comando da shell
}

void *parent_work(void *);
void *child_work(void *);

int main(int argc, char *argv[]) {
	pthread_t tid;
	long i;
	int ret;
	if (argc < 3) {
		printf("Devi inserire 2 parametri \n");
		return 1;
	}
	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666)) == -1) {
		printf("Errore apertura file %s \n", argv[1]);
		return -1;
	}
	if ((file = fdopen(fd, "w+")) == NULL) {
		printf("Errore fopening %s \n", argv[1]);
		return -1;
	}
	name = argv[1];
	N = strtol(argv[2], NULL, 10);
	if (N < 1) {
		printf("Il numero di thread deve essere maggiore di 0 \n");
		return -1;
	}
	
	//allocazione memoria condivisa
	mem_seg = malloc(sizeof(char*)*N);  //casto a puntatore a carattere moltiplicato per il numero di thread
	if (mem_seg == NULL) {
		printf("Errore allocazione \n");
		return -1;
	}
	
	//mappaggio memoria
	for (i=0; i<N; i++) {
		mem_seg[i] = (char *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
		if (mem_seg[i] == NULL) {
			printf("Errore mmap \n");
			return -1;
		}
	}
	
	//creazione semaforo di dimensione N
	sd1 = semget(IPC_PRIVATE, N, IPC_CREAT | 0666);
	if (sd1 == -1) {
		printf("Errore creazione semaforo 1 \n");
		return -1;
	}
	
	//settaggio degli N elementi a 1
	for (i=0; i<N; i++) {
		ret = semctl(sd1, i, SETVAL, 1);
		if (ret == -1) {
			printf("Errore settaggio del semaforo 1 \n");
			return -1;
		}
	}
	
	//secondo semaforo
	sd2 = semget(IPC_PRIVATE, N, IPC_CREAT | 0666);
	if (sd2 == -1) {
		printf("Errore creazione semaforo 2 \n");
		return -1;
	}
	
	//settaggio degli N elementi a 0
	for (i=0;i<N;i++) {
		ret = semctl(sd2, i, SETVAL, 0);
		if (ret == -1) {
			printf("Errore settaggio del semaforo 2 \n");
			return -1;
		}
	}
	
	//creazione processo figlio
	pid = fork();
	if (pid == -1) {
		printf("Errore fork \n");
		return -1;
	}
	if (pid == 0) { //sono nel figlio (B)
		signal(SIGINT, child_handler); //gestione del segnale SIGINT (Interrupt ^C)
		for (i=0; i<N; i++) {
			if (pthread_create(&tid, NULL, child_work, (void *) i) == -1) {
				printf("Errore creazione thread \n");
				return -1;
			}
		}
	}
	else {
		signal(SIGINT, parent_handler); //gestione del segnale SIGINT
		for (i=0; i<N; i++) {
			if (pthread_create(&tid, NULL, parent_work, (void *) i) == -1) {
				printf("Errore creazione thread \n");
				return -1;
			}
		}
	}
	while(1) pause();
}

//funzione eseguita dal thread creato dal processo padre
void *parent_work(void *arg) {
	//ciascun thread deve leggere stringhe da stdin e comunicarle all'iesimo thread di B
	long me; //l'indice che identifica l'i-esimo thread
	struct sembuf oper;
	int ret;
	
	me = (long) arg;
	printf("Parent worker %d started up \n", me);
	
	oper.sem_num = me;
	oper.sem_flg = 0;
	
	while (1) {
		oper.sem_op = -1; //l'operazione è prendere un gettone
		
redo1: //prendo gettone da sd1
		ret = semop(sd1, &oper, 1);//esecuzione del comando specificato in oper da sem_op sul semaf sd1
		if (ret == -1 && errno != EINTR) {
			printf("Errore nella semop \n");
			exit(-1);
		}
		if (ret == -1) goto redo1;

redos: //prendo in input e scrivo su mem_seg[i]
		ret = scanf("%s", mem_seg[me]); //scrivo su mem_seg[i]
		if (ret == EOF && errno != EINTR) {
			printf("Errore nella scanf \n");
			exit(-1);
		}
		if (ret == -1) goto redos;
		printf("Parent worker - thread %d wrote string %s \n", me, mem_seg[me]);
		oper.sem_op = 1; //l'operazione è aggiungere un gettone
		
redo2: //avviso l'i-esimo thread di B
		ret = semop(sd2, &oper, 1); //aggiungo un gettone all'i-esimo elemento del semaf sd2
		if (ret == -1 && errno != EINTR) {
			printf("Errore nella semop \n");
			exit(-1);
		}
		if (ret == -1) goto redo2;
	}
	return NULL;
}

//funzione eseguita dal thread creato dal processo figlio
void *child_work(void *arg) {
	//ogni thread scriverà su una nuova linea del file F la stringa comunicata dal thread di A
	long me;
	int ret;
	struct sembuf oper;
	
	me = (long) arg;
	
	printf("Child worker %d started up \n", me);
	
	oper.sem_num = me;
	oper.sem_flg = 0;
	
	while(1) {
		
redo1: //prendo gettone
		oper.sem_op = -1; 
		ret = semop(sd2, &oper, 1); //prelevo un gettone
		if (ret == -1 && errno != EINTR) {
			printf("Errore prelazione gettone sem2 \n");
			exit(-1); //termina il programma (return invece termina una funzione)
		}
		if (ret == -1) goto redo1;
		
	//scrivo su file
	printf("Child worker - thread %d found string %s\n",me,mem_seg[me]);
	fprintf(file, "%s \n", mem_seg[me]); //scrivo su file
	fflush(file); //buona prassi per svuotare il buffer di output ed assicurarsi che sia tutto scritto
		
redo2: //devo aggiungere un gettone a sd1
		oper.sem_op = 1;
		ret = semop(sd1, &oper, 1);
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop thread figlio di B \n");
			exit(-1);
		}
		if (ret == -1) goto redo2;
	}
		
	return NULL;
}