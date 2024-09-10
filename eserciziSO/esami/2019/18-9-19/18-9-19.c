/*
	1)in input da argv[] un insieme di stringhe S_1 ... S_n (n >= 1)
	2)per ogni stringa S_i attivare un thread T_i
	3)main thread legge indefinitamente stringhe da stdin
	4)ogni stringa letta viene resa disponibile al thread T_1 che
	  dovrà eliminare dalla stringa ogni carattere presente in S_1
	  e sostituirlo col carattere 'spazio'
	5)successivamente T_1 rende la stringa modificata a T_2 che 
	  dovrà sostituire ogni carattere presente in S_2 della stringa modificata
	  con il carattere 'spazio'
	6)e la stessa cosa così via con gli altri thread fino a T_n
	7)T_n una volta conclusa la sua operazione dovrà passare la stringa
	  al thread OUTPUT
	8)il thread OUTPUT stampa la stringa ricevuta sul file di output, dal
	  nome output.txt
	9)i thread lavorano in pipeline, quindi sono ammesse operazioni concorrenti
	  su differenti stringhe lette dal main thread
	10)SIGINT: quando viene colpito esso dovrà stampare il contenuto corrente 
	  del file output.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>

#define SIZE 128

int sd, sd1;
FILE *file;
long i;
int num_thread;
char **strings; //le stringhe assegnate ad ogni thread
char **shmem;

void *t_func(void *arg) {
	long me = (long) arg;  //parte da 0
	struct sembuf oper;
	int i, j;
	int ret;
	char *s;
	
	if (me < num_thread-1) {
		s = strings[me];
		printf("Thread %ld avviato - in carica della stringa: %s \n", me, strings[me]);
	} else {
		printf("Thread OUTPUT avviato - in carica del file di output \n");
	}
	
	while(1) {
		//prendo un gettone da sd[me]
		oper.sem_num = me;
		oper.sem_flg = 0;
		oper.sem_op = -1;
redo1:
		ret = semop(sd, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (3) \n");
			exit(1);
		}
		
		if(me == num_thread-1) {
			printf("Scrivendo la stringa %s sul file di output...\n", shmem[me]);
			fprintf(file, "%s \n", shmem[me]);
			fflush(file);
			oper.sem_num = 0;
			oper.sem_flg = 0;
			oper.sem_op = 1;
redo2:
			ret = semop(sd1, &oper, 1);
			if (ret == -1 && errno == EINTR) goto redo2;
			if (ret == -1 && errno != EINTR) {
				printf("Errore semop (5) \n");
				exit(1);
			}
		} else {
			//sostituzione carattere: sostituire ogni carattere di shmem[me] presente in strings[me]
			//col carattere spazio
			printf("Thread %ld possiede la stringa: %s \n", me, strings[me]);
			for (i=0; i<strlen(shmem[me]); i++) {
				for (j=0; j<strlen(s); j++) {
					if (shmem[me][i] == s[j]) {
						shmem[me][i] = ' ';
					}
				}
			}
			printf("La stringa adesso e': %s \n",shmem[me]);
			shmem[me+1] = shmem[me];
			
			oper.sem_num = me+1;
			oper.sem_flg = 0;
			oper.sem_op = 1;
redo3:
			ret = semop(sd, &oper, 1);
			if (ret == -1 && errno == EINTR) goto redo3;
			if (ret == -1 && errno != EINTR) {
				printf("Errore semop (4) \n");
				exit(1);
			}
		
		}
	}
	
	return NULL;
}

void printer(int dummy) {
	system("cat output.txt \n");
}

int main(int argc, char *argv[]) {
	int ret;
	pthread_t tid;
	struct sembuf oper;
	
	if (argc < 2) {
		printf("Numero argomenti passati insufficiente \n");
		return -1;
	}
	
	num_thread = argc; //deve includere anche il thread output quindi mi serve un numero in più
	strings = argv+1;
	
	file = fopen("output.txt", "w+");
	if (file == NULL) {
		printf("Errore apertura file \n");
		return -1;
	}
	
	shmem = malloc(sizeof(char*)*num_thread);
	if (shmem == NULL) {
		printf("Errore malloc shmem \n");
		return -1;
	}
	for (i=0; i<num_thread; i++) {
		shmem[i] = (char *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
		if(shmem[i] == NULL) {
			printf("Errore mmap shmem \n");
			return -1;
		}
	}
	
	//array di semafori per avvisare l'i-esimo thread
	sd = semget(IPC_PRIVATE, num_thread, IPC_CREAT | IPC_EXCL | 0666);
	if (sd == -1) {
		printf("Errore creazione semaforo (1) \n");
		return -1;
	}
	for (i=0; i<num_thread; i++) {
		ret = semctl(sd, i, SETVAL, 0);
	}
	if (ret == -1) {
		printf("Errore inizializzazione semaforo (1) \n");
		return -1;
	}
	
	//semaforo per gestire la scrittura da stdin
	sd1 = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (sd1 == -1) {
		printf("Errore creazione semaforo (2) \n");
		return -1;
	}
	ret = semctl(sd1, 0, SETVAL, 1);
	if (ret == -1) {
		printf("Errore inizializzazione semaforo (2) \n");
		return -1;
	}
	
	
	//per ogni stringa attivare un thread
	for (i=0; i<num_thread; i++) {
		ret = pthread_create(&tid, NULL, t_func, (void *) i);
		if (ret == -1) {
			printf("Errore creazione thread \n");
			return -1;
		}
	}
	
	//signal
	signal(SIGINT, printer);
	
	
	while(1) {
		oper.sem_num = 0;
		oper.sem_flg = 0;
		oper.sem_op = -1;
		
redo1:  //prendo un gettone da sd1
		ret = semop(sd1, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo1;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (1) \n");
			exit(1);
		}
		
redo2:  //acquisisco stringa in input e la salvo su shmem[0]
		ret = scanf("%s", shmem[0]);
		if (ret == EOF && errno == EINTR) goto redo2;
		if (ret == EOF && errno != EINTR) {
			printf("Errore scanf \n");
			exit(1);
		}
		printf("Ho letto %s \n", shmem[0]);
		
		//modifico operazione da fare, devo avvisare il primo thread
		oper.sem_num = 0;
		oper.sem_flg = 0;
		oper.sem_op = 1;
		
redo3:  //inserisco un gettone al primo elemento dell'array di semafori
		ret = semop(sd, &oper, 1);
		if (ret == -1 && errno == EINTR) goto redo3;
		if (ret == -1 && errno != EINTR) {
			printf("Errore semop (2) \n");
			exit(1);
		}
	}
	
	return 0;
}