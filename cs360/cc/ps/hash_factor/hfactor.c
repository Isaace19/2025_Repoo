#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "hfactor.h"

int hfactor_process(uint64_t target)
{
 /*
  *	used for fork'd children, goal is to pop a message from the message queue and test if 
  *	its a factor of the target
  *
  * if yes -> push into shared memory (requires semaphore)
  * no -> nothing happens (chudcel)
  *
  * values of 0 will act as sentinal values to trigger the children to return to quit.
  *
  * close all resources made in each child
  * return 0 on success, -1 for errors
  * */
	mqd_t q = mq_open(QUEUE_NAME, O_RDONLY);
	sem_t *sem = sem_open(SEMAPHORE_NAME, 0);
	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

	if (q == (mqd_t)-1 || sem == SEM_FAILED || shm_fd == -1) {
		perror("child IPC open");
		return -1;
	}

//	struct shared_mem {
//		uint64_t size;
//		uint64_t data[];
//	} *shared_mem


	uint64_t *shared_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
			                            MAP_SHARED, shm_fd, 0);


	if (shared_mem == MAP_FAILED) {
		perror("mmap");
		mq_close(q);
		sem_close(sem);
		close(shm_fd);
		return -1;
	}

	uint64_t candidate;
	unsigned int prio;
	ssize_t bytes;

	for (;;) {
		bytes = mq_receive(	q, (char *)&candidate, sizeof(candidate), &prio);
		if (bytes < 0) {
			perror("mq_receieve");
			break;
		}	

		// sentinel value of 0 used for children flooding the queue.
		if (candidate == 0) {
			break;
		}
		// candidate for /2
		if (target % candidate == 0) {
			sem_wait(sem);
		
			uint64_t size = shared_mem[0];

			shared_mem[size] = candidate;
			sem_post(sem);
		}
	}

	// cleanup 

	munmap(shared_mem, 4096);
	mq_close(q);
	sem_close(sem);
	close(shm_fd);
	return 0;
}	
	
int hfactor_init(mqd_t *q, sem_t **sem, int *shm)
{
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(uint64_t);

    *q = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (*q == (mqd_t)-1) {
        perror("mq_open");
        return -1;
    }

    *sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (*sem == SEM_FAILED) {
        perror("sem_open");
        mq_unlink(QUEUE_NAME);
        return -1;
    }

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        sem_close(*sem);
        sem_unlink(SEMAPHORE_NAME);
        mq_unlink(QUEUE_NAME);
        return -1;
    }

    if (ftruncate(fd, 4096) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        close(fd);
        sem_close(*sem);
        sem_unlink(SEMAPHORE_NAME);
        mq_unlink(QUEUE_NAME);
        return -1;
    }

    *shm = fd;
    return 0;
}
void hfactor_deinit(mqd_t q, sem_t *sem, int shm)
{
	// include the close and unlinking stage of this program into the hfactor_deinit.
	
	mq_close(q);
	mq_unlink(QUEUE_NAME);

	sem_close(sem);
	sem_unlink(SEMAPHORE_NAME);

	// close the shared memory
	close(shm);
	shm_unlink(SHM_NAME);	
	// add in a free statement if necessary	
}

int hfactor_report(const char *reporter, int shm)
{
	int ptc[2]; // parent to child pipe
	int ctp[2]; // child to parent pipe

	pid_t pid = fork();

	pipe(ptc); // initialize pipe to child
	pipe(ctp); // initialize child to pipe
	
	if (pipe(ptc) == -1 || pipe(ctp) == -1) {
		perror("pipe");
		return -1;
	}
	
	if (pid < 0) {
		perror("fork");
		// error check close pipes
		close(ptc[0]);
		close(ptc[1]);
		close(ctp[0]);
		close(ctp[1]);

		return -1;	
	}

	/* CHILD PROCESS */
	if(pid == 0){
		
		close(ptc[1]);
		close(ctp[0]);
	
		// redirect the stdin to the parent to read and the fileout to ctp 
		dup2(ptc[0], STDIN_FILENO);
		dup2(ctp[1], STDOUT_FILENO);

		close(ptc[0]);
		close(ctp[1]);

		execl(reporter, "reporter", NULL);
		perror("excel");
		_exit(1);
	} 
	/* PARENT PROCESS */
	close(ptc[0]);
	close(ctp[1]);
	
	uint64_t *shared_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

	if (shared_mem == MAP_FAILED) {
		perror("mmap");
		close(ptc[1]);
		waitpid(pid, NULL, 0);
		return -1;
	}

	uint64_t count = 0;
	lseek(shm, 0, SEEK_SET);
	read(shm, &count, sizeof(count));

	for (uint64_t i = 0; i < count; i++) {
		uint64_t factor;
		lseek(shm, sizeof(uint64_t) + i * sizeof(uint64_t), SEEK_SET);
		read(shm, &factor, sizeof(factor));
		write(ptc[1], &factor, sizeof(factor));	
	}

	// clean
	munmap(shared_mem, 4096);
	close(ptc[1]);

	close(ctp[0]);
	waitpid(pid, NULL, 0);
	return 0;
}

int hfactor_calculate(const char *reporter, uint64_t target, uint64_t num_workers)
{
	mqd_t q;
	sem_t *sem;
	int shm_fd;
	
	if (hfactor_init(&q, &sem, &shm_fd) == -1) {
		fprintf(stderr, "IPC init failed\n");
		return -1;
	}

	 if (q == (mqd_t)-1 || sem == SEM_FAILED || shm_fd == -1) {
		perror("child IPC open");
		return -1;
	}

	for(uint64_t i = 0; i < num_workers; i++) {
		pid_t pid = fork();
		if(pid < 0){
			perror("fork");
			return -1;
		}

		// work on the child process
		if (pid == 0) {
			if (hfactor_process(target) == -1) {
				_exit(1);
			}
			_exit(0);
		}
	}
	// populate the message queue
	for (uint64_t i = 2; i <= target/2; i++) {
		mq_send(q, (const char*)&i, sizeof(i), 0);
	}

	// work on the parent process
	uint64_t quit = 0;
	for (uint64_t i; i < num_workers; i++) {
		mq_send(q, (const char*)&quit, sizeof(quit), 0);
	}

	// wait on all children to collect them
	while (waitpid(-1, NULL, 0) > 0)
		;

	
	// report results func
	if (hfactor_report(reporter, shm_fd) == -1) {
		fprintf(stderr, "hfactor_report failed\n");	}



	// cleanup on ailse 7
	hfactor_deinit(q, sem, shm_fd);

	return 0;
}

