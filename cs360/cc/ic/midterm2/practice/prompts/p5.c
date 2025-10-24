/*
 * reporter.c  –  Advanced IPC + dup + exec practice
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

#define QNAME "/workq"
#define SHMNAME "/resultmem"
#define SEMNAME "/mutex"

int main(void) {
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);

    // TODO: create mq, shm, sem
    // TODO: initialize *shared_total = 0

    pid_t worker = fork();
    if (worker == 0) {
        // --- WORKER CODE ---
        // receive ints, update shared memory under semaphore
        exit(0);
    }

    pid_t reporter = fork();
    if (reporter == 0) {
        // --- REPORTER CODE ---
        // lock semaphore, read shared_total, write to temporary FD,
        // dup2() that FD to STDIN, execl("/bin/cat", "cat", "/proc/self/fd/0", NULL);
        exit(0);
    }

    // --- PARENT CODE ---
    // send 1..N then sentinel 0 to queue
    // wait for both children
    // cleanup all IPC resources

    return 0;
}

