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
     * Child process function:
     * - Read candidate from message queue
     * - If divides target, write into shared memory (protected by semaphore)
     * - Stop on sentinel value 0
     */
    mqd_t q = mq_open(QUEUE_NAME, O_RDONLY);
    sem_t *sem = sem_open(SEMAPHORE_NAME, 0);
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

    if (q == (mqd_t)-1 || sem == SEM_FAILED || shm_fd == -1) {
        perror("child IPC open");
        return -1;
    }

    uint64_t *shared_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        mq_close(q);
        sem_close(sem);
        close(shm_fd);
        return -1;
    }

    uint64_t candidate;
    ssize_t bytes;

    for (;;) {
        bytes = mq_receive(q, (char *)&candidate, sizeof(candidate), NULL);
        if (bytes < 0) {
            perror("mq_receive");
            break;
        }

        if (candidate == 0)
            break;

        if (target % candidate == 0) {
            sem_wait(sem);

            uint64_t count = shared_mem[0];
            shared_mem[count + 1] = candidate;
            shared_mem[0] = count + 1;

            sem_post(sem);
        }
    }

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

    // Initialize shared memory counter to zero
    uint64_t *mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem != MAP_FAILED) {
        mem[0] = 0;
        munmap(mem, 4096);
    }

    *shm = fd;
    return 0;
}

void hfactor_deinit(mqd_t q, sem_t *sem, int shm)
{
    mq_close(q);
    mq_unlink(QUEUE_NAME);

    sem_close(sem);
    sem_unlink(SEMAPHORE_NAME);

    close(shm);
    shm_unlink(SHM_NAME);
}

int hfactor_report(const char *reporter, int shm)
{
    int ptc[2]; // parent -> child
    int ctp[2]; // child -> parent

    if (pipe(ptc) == -1 || pipe(ctp) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(ptc[0]); close(ptc[1]);
        close(ctp[0]); close(ctp[1]);
        return -1;
    }

    if (pid == 0) {
        // Child process
        close(ptc[1]);
        close(ctp[0]);

        dup2(ptc[0], STDIN_FILENO);
        dup2(ctp[1], STDOUT_FILENO);

        close(ptc[0]);
        close(ctp[1]);

        execl(reporter, reporter, NULL);
        perror("execl");
        _exit(1);
    }

    // Parent process
    close(ptc[0]);
    close(ctp[1]);

    uint64_t *shared_mem = mmap(NULL, 4096, PROT_READ, MAP_SHARED, shm, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap in hfactor_report");
        close(ptc[1]);
        waitpid(pid, NULL, 0);
        close(ctp[0]);
        return -1;
    }

    uint64_t count = shared_mem[0];
    write(ptc[1], &count, sizeof(count));

    for (uint64_t i = 1; i <= count; i++)
        write(ptc[1], &shared_mem[i], sizeof(uint64_t));

    munmap(shared_mem, 4096);
    close(ptc[1]);

    waitpid(pid, NULL, 0);
    close(ctp[0]);

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

    for (uint64_t i = 0; i < num_workers; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        }
        if (pid == 0) {
            if (hfactor_process(target) == -1)
                _exit(1);
            _exit(0);
        }
    }

    // Fill message queue
    for (uint64_t i = 2; i <= target / 2; i++)
        mq_send(q, (const char *)&i, sizeof(i), 0);

    // Send sentinel 0s to all workers
    uint64_t quit = 0;
    for (uint64_t i = 0; i < num_workers; i++)
        mq_send(q, (const char *)&quit, sizeof(quit), 0);

    // Wait for all children
    while (waitpid(-1, NULL, 0) > 0)
        ;

    // Generate report
    if (hfactor_report(reporter, shm_fd) == -1)
        fprintf(stderr, "hfactor_report failed\n");

    hfactor_deinit(q, sem, shm_fd);
    return 0;
}
