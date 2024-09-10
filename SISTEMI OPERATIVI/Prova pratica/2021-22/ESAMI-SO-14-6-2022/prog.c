/******************************************************************
Welcome to the Operating System examination

You are editing the '/home/esame/prog.c' file. You cannot remove 
this file, just edit it so as to produce your own program according to
the specification listed below.

In the '/home/esame/'directory you can find a Makefile that you can 
use to compile this program to generate an executable named 'prog' 
in the same directory. Typing 'make posix' you will compile for 
Posix, while typing 'make winapi' you will compile for WinAPI just 
depending on the specific technology you selected to implement the
given specification. Most of the required header files (for either 
Posix or WinAPI compilation) are already included in the head of the
prog.c file you are editing. 

At the end of the examination, the last saved snapshot of this file
will be automatically stored by the system and will be then considered
for the evaluation of your exam. Modifications made to prog.c which are
not saved by you via the editor will not appear in the stored version
of the prog.c file. 
In other words, unsaved changes will not be tracked, so please save 
this file when you think you have finished software development.
You can also modify the Makefile if requesed, since this file will also
be automatically stored together with your program and will be part
of the final data to be evaluated for your exam.

PLEASE BE CAREFUL THAT THE LAST SAVED VERSION OF THE prog.c FILE (and of
the Makfile) WILL BE AUTOMATICALLY STORED WHEN YOU CLOSE YOUR EXAMINATION 
VIA THE CLOSURE CODE YOU RECEIVED, OR WHEN THE TIME YOU HAVE BEEN GRANTED
TO DEVELOP YOUR PROGRAM EXPIRES. 


SPECIFICATION TO BE IMPLEMENTED:
Si sviluppi una applicazione che riceva tramite argv[] la seguente linea di comando

    nome_prog -f file1 [file2] ... [fileN] -s stringa1 [stringa2] ... [stringaN] 
    
indicante N nomi di file (con N > 0) ed N ulteriori stringhe (il numero dei nomi dei 
file specificati deve corrispondere al numero delle stringhe specificate).

L'applicazione dovra' generare N processi figli concorrenti, in cui l'i-esimo di questi 
processi effettuera' la gestione dell'i-esimo dei file identificati tramite argv[].
Tale file dovra' essere rigenerato allo startup dell'applicazione.
Il main thread del processo originale dovra' leggere indefinitamente stringhe da 
standard input e dovra' comparare ogni stringa letta che le N stringhe ricevute in 
input tramite argv[].
Nel caso in cui la stringa letta sia uguale alla i-esima delle N stringhe ricevuta 
in input, questa dovra' essere comunicata all'i-esimo processo figlio in modo che questo 
la possa inserire in una linea del file di cui sta effettuando la gestione. Invece, 
se il main thread legge una stringa non uguale ad alcuna delle N stringhe ricevute 
in input, questa stringa dovra essere comunicata a tutti gli N processi figli
attivi, che la dovranno scrivere sui relativi file in una nuova linea.

L'applicazione dovra' gestire il segnale SIGINT (o CTRL_C_EVENT nel caso
WinAPI) in modo tale che quando uno qualsiasi dei processi figli venga colpito 
dovra' riportare il contenuto del file da esso correntemente gestito in un file
con lo stesso nome ma con suffisso "_backup".  Invece il processo originale non dovra'
terminare o eseguire alcuna attivita' in caso di segnalazione.

In caso non vi sia immissione di dati sullo standard input e non vi siano segnalazioni, 
l'applicazione dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

*****************************************************************/
#ifdef Posix_compile
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#else
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv){

	return 0;
}
