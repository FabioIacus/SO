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
Scrivere un programma che riceva in input tramite argv[] il nome di un file
F e una stringa S contenente un numero arbitrario di caratteri. 
Il main thread dell'applicazione dovra' creare il file F e poi leggere indefinitamente 
stringhe dallo standard input per poi scriverle, una per riga, all'interno del file.
Qualora venga ricevuto il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI), dovra'
essere lanciato un nuovo thread che riporti il contenuto del file F all'interno di un 
altro file con lo stesso nome, ma con suffisso "_shadow", sostituendo tutte le stringhe
che coincidono con la stringha S ricevuta dall'applicazione tramite argv[] con 
una stringha della stessa lunghezza costituita da una sequenza di caratteri '*'.
Il lavoro di questo thread dovra' essere incrementale, ovvero esso dovra' riportare 
sul file shadow solo le parti del file originale che non erano state riportate in 
prcedenza. Al termine di questa operazione, il thread dovra' indicare su standard 
output il numero di stringhe che sono state sostituite in tutto.

In caso non vi sia immissione di dati sullo standard input, l'applicazione 
dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

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
