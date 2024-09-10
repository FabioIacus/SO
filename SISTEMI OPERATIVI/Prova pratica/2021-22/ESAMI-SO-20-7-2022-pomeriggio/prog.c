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
Scrivere un programma che riceva in input tramite argv[] i nomi di N file,
con N magiore o uguale a 1. L'applicazione dovrà controllare che i nomi dei 
file siano diversi tra di loro.
I file dovranno essere creati oppure troncati se esistenti.
Per ogniuno dei file dovra' essere attivato un nuovo thread, che indicheremo
con Ti, che gestirà il contenuto del file. 
I thread Ti leggeranno linee di caratteri da standard input a turno secondo 
uno schema circolare, e scriveranno la linea letta all'interno del file 
da loro gestito.

L'applicazione dovrà essere in grado di gestire il segnale SIGINT 
(o CTRL_C_EVENT nel caso WinAPI) in modo tale che quando il processo  
verrà colpito riporti su standard output il contenuto corrente (ovvero le linee 
attualmente presenti) di tutti i file che erano stati specificati in argv[], 
seguendo esattamente l'ordine tramite cui le linee sono state inserite al loro interno.
In ogni caso, nessuno dei thread dovrà terminare la sua esecuzione in caso di arrivo
della segnalazione.

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
