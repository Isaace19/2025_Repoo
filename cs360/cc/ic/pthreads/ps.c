#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


// pthread specific:
static void *worker(void *arg); // mem address passed to the worker, then up to us to typecast
								// static only exists in this file
int i;

int main() {
	pthread_t tid;


	// run a sleep 
	sleep(1);

	printf("Parent starts\n");
	pthread_create(&tid, NULL, worker, NULL);
	pthread_create(&tid, NULL, worker, NULL);
	printf("Parent resumes\n");
	return 0;
}

static void *worker(void *arg) {
	// epiloge and prologue existing on the stack. TODO: figure out why this resolves in c.
	printf("Running Worker!\n");

	long j; 
	for(j = 0; j < 10000000000; j+=1){
		i += j;
	}


	exit(EXIT_SUCCESS);
	return NULL;
}
