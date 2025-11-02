#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <mqueue.h> // This is the correct POSIX header for Message Queues
#include <semaphore.h>

typedef struct {
    double x;
    double y;
    double z;
} DATA;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <mqueue> <sem> <shm>\n", argv[0]);
        return 1;
    }

    // read logic

    

    mqd_t mq = mq_open(argv[1], O_RDWR);
    sem_t *sem = sem_open(argv[2], 0);
    int shm_fd = shm_open(argv[3], O_RDWR, 0);

    ftruncate(shm_fd, sizeof(double));

    void *map = mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    double *sum = (double *)map;
    DATA EXIT_STATE = { -1, -1, -1};
    // Receive loop
    while (1) {
        DATA message;
        mq_receive(mq, (char *)&message, sizeof(message), NULL);

        // Stop on exit state
        if (message.x == EXIT_STATE.x && message.y == EXIT_STATE.y && message.z == EXIT_STATE.z) {
            break;
        }

        double computed = message.x * message.y + message.z;

        sem_wait(sem);
        double updated_sum = *sum + computed;
        *sum = updated_sum;
        sem_post(sem);
    }

    munmap(map, sizeof(double));
    close(shm_fd);
    sem_close(sem);
    mq_close(mq);

    return 0;
}

 