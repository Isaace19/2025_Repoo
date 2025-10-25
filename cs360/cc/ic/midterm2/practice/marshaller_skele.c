#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <sem_name> <shm_name> <num_workers> <worker_exec>\n", argv[0]);
        return 1;
    }

    const char *sem_name = argv[1];
    const char *shm_name = argv[2];
    int num = atoi(argv[3]);
    const char *worker = argv[4];

    /* Create semaphore (mutex style) */
    sem_t *sem = sem_open(sem_name, O_CREAT | O_EXCL, 0600, 1);
    if (sem == SEM_FAILED) { perror("sem_open"); exit(1); }

    /* Create shared memory */
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR | O_EXCL, 0600);
    if (shm_fd == -1) { perror("shm_open"); exit(1); }
    ftruncate(shm_fd, sizeof(int));
    int zero = 0;
    write(shm_fd, &zero, sizeof(zero));

    // if parent, then we have in the parent
    struct mq_attr mqa;
    mqa.mq_msgsize = sizeof(int); // depends on input data type
    mqa.mq_maxmsg = 10;




    /* Spawn workers */
    pid_t *pids = malloc(sizeof(pid_t) * num);
    for (int i = 0; i < num; i++) {
        if ((pids[i] = fork()) == 0) {
            // Child -> run worker executable
            execl(worker, worker, sem_name, shm_name, NULL);
            perror("execl");
            _exit(1);
        }
    }

    /* Wait for children */
    for (int i = 0; i < num; i++)
        waitpid(pids[i], NULL, 0);

    /* Cleanup */
    free(pids);
    sem_close(sem);
    sem_unlink(sem_name);
    close(shm_fd);
    shm_unlink(shm_name);

    return 0;
}

