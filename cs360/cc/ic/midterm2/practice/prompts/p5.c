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
    mqd_t mq = mq_open(QNAME, O_RDWR);
    sem_t *sem = sem_open(SEMNAME, 0);
    int shm_fd = shm_open(SHMNAME, O_RDWR, 0);
    // implement shared memroy between things.
    
    ftruncate(shm_fd, sizeof(int));
    void *shared_mem = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    int *sum = (int *)shared_mem;

    shared_mem = 0;
    int i;
    pid_t worker = fork();
    if (worker == 0) {
        // --- WORKER CODE ---
        // receive ints, update shared memory under semaphore
        for(;;) {
            int n;
            if(mq_receive(mq, (char*)&n, sizeof(n), NULL) == -1) {
                perror("mq_receive");
                _exit(1);
            }
            if (n == 0) {
                break;
            }
            sem_wait(sem);
            *shared_total += n;
            sem_post(sem);
        } 

        exit(0);
    }

    pid_t reporter = fork();
    if (reporter == 0) {
        // --- REPORTER CODE ---
        // lock semaphore, read shared_total, write to temporary FD,
        // dup2() that FD to STDIN, execl("/bin/cat", "cat", "/proc/self/fd/0", NULL);
        // Allow worker to finish updating
        sleep(1);
        sem_wait(sem);
        int result = *shared_total;
        sem_post(sem);

        // Write result to a temporary file
        char tmpname[] = "/tmp/ipc_reportXXXXXX";
        int tmpfd = mkstemp(tmpname);
        if (tmpfd == -1) { perror("mkstemp"); _exit(1); }

        dprintf(tmpfd, "%d\n", result);
        lseek(tmpfd, 0, SEEK_SET);

        // Redirect stdin to this file
        dup2(tmpfd, STDIN_FILENO);
        close(tmpfd);

        // Reporter uses exec to print via /bin/cat
        execl("/bin/cat", "cat", "/proc/self/fd/0", NULL);
        perror("execl");

        exit(0);
    }

    // --- PARENT CODE ---
    // send 1..N then sentinel 0 to queue
    // wait for both children
    // cleanup all IPC resources
    for (int i = 1; i <= N; i++) {
        mq_send(mq, (const char*)&i, sizeof(i), 0);
    }
    int sentinel = 0;
    mq_send(mq, (const char*)&sentinel, sizeof(sentinel), 0);

    // Wait for both children
    waitpid(worker, NULL, 0);
    waitpid(reporter, NULL, 0);

    /* === Cleanup === */
    munmap(shared_total, sizeof(int));
    close(shm_fd);
    sem_close(sem);
    mq_close(mq);

    shm_unlink(SHMNAME);
    sem_unlink(SEMNAME);
    mq_unlink(QNAME);

    return 0;
}

