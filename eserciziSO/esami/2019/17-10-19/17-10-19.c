/*	1)ricevere da argv[] i nomi di N file (N >= 1)
	2)per ogni nome ricevuto dovrà essere attivato un nuovo thread
	3)il main dovrà leggere indefinitamente stringhe dallo standard-input
	4)il main dovrà rendere ogni stringa letta, disponibile ad un solo thread secondo uno schema circolare
	5)ciascun thread T_i per ogni stringa letta dovrà scriverla su una nuova linea del file F_i
	6)gestire il segnale SIGINT in modo tale che:
		quando il processo viene colpito esso riversa su stdout e su un apposito file chiamato "output-file"
		il contenuto di tutti i file gestiti dall'applicazione 
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

FILE *output_file;
FILE **source_file;
long N;
char **files;
char **buffer;
pthread_mutex_t *ready;
pthread_mutex_t *done;

//riversa su stdout e su un apposito file chiamato "output-file"
//il contenuto di tutti i file gestiti dall'applicazione
void printer(int dummy) {
	int i;
	char *p;
	int ret;
	
	for (i=0;i<N;i++) {
		source_file[i] = fopen(files[i], "r"); //source_file[i] punta a files[i]
		if (source_file[i] == NULL) {
			printf("Errore apertura file %s \n", source_file[i]);
			exit(EXIT_FAILURE);
		}
	}
	
	output_file = fopen("output_file", "w+"); //crea il file e lo apre in lettura/scrittura, se esiste cancella il contenuto
	if (output_file == NULL) {
		printf("Errore apertura file %s \n", output_file);
		exit(EXIT_FAILURE);
	}
	
	i=0;
	while(1) {
		ret = fscanf(source_file[i], "%ms", &p); //legge da source_file[i] e scrive su p
		if (ret == EOF) break;
		printf("%s \n", p);
		fflush(stdout);
		fprintf(output_file, "%s \n", p); //scrivo su output_file il contenuto di p
		fflush(output_file);
		free(p);
		i = (i+1)%N;
	}
}

//ciascun thread T_i per ogni stringa letta dovrà scriverla su una nuova linea del file F_i
void *func(void *arg) {
	sigset_t set; //insieme di segnali
	long int me = (long int) arg;
	int i, j;
	FILE *target_file;
	
	printf("Thread %d started up - in charge of %s \n", me, files[me]);
	fflush(stdout);
	
	target_file = fopen(files[me], "w+"); //crea/apre un nuovo file in lettura/scrittura
	if (target_file == NULL) {
		printf("Errore apertura file: %s \n", files[me]);
		exit(EXIT_FAILURE);
	}
	
	sigfillset(&set); //inserisce tutti i segnali in set
	sigprocmask(SIG_BLOCK, &set, NULL); //gestione della signal mask (con SIG_BLOCK i segnali indicati in set vengono aggiunti alla signal mask)
	
	while(1) {
		if (pthread_mutex_lock(ready+me)) {
			printf("Errore mutex lock \n");
			exit(EXIT_FAILURE);
		}
		
		printf("Thread %d - got string %s \n", me, buffer[me]);
		fprintf(target_file, "%s \n", buffer[me]);
		fflush(target_file);
		
		if(pthread_mutex_unlock(done+me)) {
			printf("Errore mutex unlock \n");
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char *argv[]) {
	int ret;
	long i;
	char *p;
	pthread_t tid;
	
	if (argc < 2) {
		printf("Numero argomenti troppo basso \n");
		return -1;
	}
	
	printf("Numero di thread da attivare: %d \n",argc-1);
	N = argc-1;
	
	files = argv+1; //files punta ad argv+1 quindi la stringa dopo il nome del file
	printf("%s \n", *files);
	
	//malloc di un buffer
	buffer = (char**)malloc(sizeof(char*) * N);
	if (buffer == NULL) {
		printf("Errore malloc \n");
		return -1;
	}
	
	//malloc di source_file
	source_file = (FILE**)malloc(sizeof(FILE*) * N);
	if (source_file == NULL) {
		printf("Errore allocazione file pointer \n");
		return -1;
	}
	
	//malloc di ready e done (mutex di N elementi)
	ready = malloc(N * sizeof(pthread_mutex_t));
	done = malloc(N * sizeof(pthread_mutex_t));
	if (ready == NULL || done == NULL) {
		printf("Errore malloc ready o done \n");
		return -1;
	}
	
	//inizializzo N mutex
	for (i=0;i<N;i++) {
		if (pthread_mutex_init(ready+i, NULL) || pthread_mutex_init(done+i, NULL) || pthread_mutex_lock(ready+i)) {
			printf("Errore inizializzazione mutex \n");
			return -1;
		}
	}
	
	//creo N thread
	for (i=0;i<N;i++) {
		if (pthread_create(&tid, NULL, func, (void *) i)) {
			printf("Errore creazione thread \n");
			return -1;
		}
	}
	
	//gestione segnale SIGINT
	signal(SIGINT, printer);
	
	//devo rendere ogni stringa letta da stdin disponibile ad uno solo degli N thread
	//secondo uno schema circolare
	i=0;
	while(1) {
		
read_again: //leggo stringhe in maniera indefinita da stdin
			ret = scanf("%ms", &p);
			if (ret == EOF && errno == EINTR) goto read_again;
			printf("Stringa letta (l'area è all'indirizzo %p): %s \n",p,p);
			
redo_1:
			if(pthread_mutex_lock(done+i)) { //l'i-esimo thread locka mutex done
				if (errno == EINTR) goto redo_1;
				printf("Errore nel lock del mutex \n");
				exit(EXIT_FAILURE);
			}
			buffer[i] = p; //scrivo su buffer[i]

redo_2: 
			if (pthread_mutex_unlock(ready+i)) { //l'i-esimo thread unlocka mutex ready
				if (errno == EINTR) goto redo_2;
				printf("Errore unlock del mutex \n");
				exit(EXIT_FAILURE);
			}
	i = (i+1)%N; //schema circolare
	}
	
	return 0;
}