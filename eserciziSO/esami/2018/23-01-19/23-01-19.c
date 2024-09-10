/*
	1) Tramite argv[] il nome di un file F e N stringhe s1...sN (N>=1)
	2) Per ogni stringa attivare un thread Ti che sarà un gestore della stringa si
	3) Il main legge stringhe indefinitamente da stdin
	4) Ogni nuova stringa viene comunicata a tutti i thread tramite buffer condiviso
	5) Ciascun thread controlla se la stringa è uguale a quella gestita da lui stesso
	6) In caso positivo, ogni carattere della stringa immessa viene sostituito col carattere '*'
	7) Dopo il controllo, la stringa modificata o non, viene scritta dal main su una nuova linea del file FP_OFF
	8) SIGINT: stampare su stdout il contenuto di F
*/

