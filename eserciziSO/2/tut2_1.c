#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

//prendi una stringa in input (anche con gli spazi bianchi) e salvarla in un buffer
//forkare un processo figlio che manda in stampa la stessa stringa acquisita dal processo padre	
//il padre termina solo dopo che il processo figlio ha terminato (verificare l'ordine stampando i PID)
 
int main() {
	//BUFFER NELL'HEAP
	char *buffer;
	scanf("%m[^\n]", &buffer);
	
	//BUFFER NELLO STACK
	//char buffer[1024];
	//scanf("%[^\n]", buffer); //prendo tutta la frase
	//scanf("%s", buffer); //prendo solo la prima parola

	pid_t pid;
	int status;
	pid = fork();

	if (pid == -1) {
		printf("Errore \n");
		exit(-1);
	}
	
	if (pid == 0) {    //sono nel figlio
		printf("Il processo con PID = %d ha acquisito la stringa: %s \n", getpid(), buffer);
		exit(0);
	}	
	else {   //sono nel padre
		wait(&status);   //aspetta la terminazione di un generico processo figlio
		printf("Il processo con PID = %d ha terminato con successo \n", getpid());
	}

	free(buffer);
}
