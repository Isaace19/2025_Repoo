/*
 * Prompt 4: Multi Process and IPC
 * Compile prompt: gcc -o p4 p4.c -lrt -pthread
 * */

/*
 * prodcons.c
 * Multi-process producer/consumer using POSIX IPC
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <semaphore.h>
#include <string.h>

#define QNAME "/taskqueue"
#define SHMNAME "/results"
#define SEMNAME "/lock"

int main(void) {
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);

    mqd_t mq;
    int *shared_total;

    // 1. Create IPC objects
    // TODO: mq_open, shm_open, ftruncate, mmap, sem_open
    mq = mq_open(QNAME, O_CREAT | O_RDWR, 0666, &attr);
    sem_t *sem = sem_open(SEMNAME, O_CREAT, 0666, 1); // semaphore initialized to 1 for mutual exclusion
    int shm_fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, sizeof(int));
    shared_total = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shared_total == MAP_FAILED) {
        perror("mmap");
        exit(1);
    } 

    // 2. Initialize total = 0 in shared memory
    // TODO
    *shared_total = 0;

    pid_t pid = fork();
    if (pid == 0) {
        // ---- PRODUCER ----
        for (int i = 1; i <= 10; i++) {
            // TODO: send integer i to queue
            // for the num of process, being the 10 message sizies, we want to send these to the receiver.
            mq_send(mq, (const char*)&i, sizeof(i), 0);
        }
        // Send sentinel 0
        // TODO
        // should be some sort of loop that detects when the process is over, however I don't know
        int sentinel = 0;
        mq_send(mq, (const char*)&sentinel, sizeof(sentinel), 0);
        mq_close(mq);
        sem_close(sem);
        close(shm_fd);
        munmap(shared_total, sizeof(int));
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // ---- CONSUMER ----
        while (1) {
            int n;
            if(mq_receive(mq, (char *)&n, sizeof(n), NULL) == -1) {
                perror("mq_receive");
                exit(1);
            }
            if (n == 0) break;
            // TODO: lock semaphore, update *shared_total += n, unlock
            sem_wait(sem);
            *shared_total +=n;
            sem_post(sem);
        }
        mq_close(mq);
        sem_close(sem);
        close(shm_fd);
        munmap(shared_total, sizeof(int));
        exit(0);
    }

    // ---- PARENT ----
    waitpid(pid, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Final total: %d\n", *shared_total);

    // ---- Cleanup ----
    // TODO: close/unlink mq, sem, shm
    munmap(shared_total, sizeof(int));
    close(shm_fd);
    mq_close(mq);
    sem_close(sem);


    shm_unlink(SHMNAME);
    sem_unlink(SEMNAME);
    mq_unlink(QNAME);


    return 0;
}

