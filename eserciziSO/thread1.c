#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>

void *child_func(void *par){
	int i = *((int*) par);
	sleep(2);
	printf("I'm the child #");
	fflush(stdout);
	if (i) printf("%d! \n", (*(int*) (NULL)));
	else printf("%d! \n", i);
	*((int*)par) = 0xf;
	pthread_exit(par);
}

int main() {
	int res, param, *status = NULL;
	pthread_t ctid;
	for (int i=0; i<2; i++) {
		param = i;
		printf("I'm a thread and I'm going to create a child #%d \n", i);
		res = pthread_create(&ctid, NULL, child_func, &param);
		printf("I'm now a parent and I'll wait for my child #%d to die... \n", i);
		pthread_join(ctid, (void**)&status);
		printf("My child #%d has terminated and it invoked pthread_exit(%d) \n", i, *((int*)status));
	}
	exit(0);
}
