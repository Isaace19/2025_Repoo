#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct {
	double x;
	double y;
	double z;
} DATA;

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <mqueue> <semaphore> <shared_memory>\n", argv[0]);
		return 1;
	}

	// read logic, have to initialize the variables of message queues, semaphore objects
	// as well as creating the shared memory file. 
	
	// starting initialiation by what I understand first, then aligned for arg. vec.

	mqd_t mq = mq_open(argv[1], )

	sem_t *sem = sem_open(argv[2], O_EXCL, 600, NULL);
}
