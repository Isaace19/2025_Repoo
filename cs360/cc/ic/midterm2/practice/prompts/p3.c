/* Prompt 3: Shared Memory with a Semaphore
 * Write a program that performs the following:
 * 1. Parent is initalized with shared memory segment and semaphore
 * 2. Two children will increment a shared counter 100 times each with sem_wait() & sem_post()
 * protection.
 *
 * compilation command: gcc -o p3 p3.c -lrt -pthread
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>

#define SHM_NAME "/myshm"
#define SEM_NAME "/mysem"

int main(void) {
    int shm_fd;
    int *counter;
    sem_t *sem;

    // 1. Create and map shared memory
    // TODO
    

    // 2. Create and initialize semaphore
    // TODO

    // 3. Fork two children
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child code
            for (int j = 0; j < 1000; j++) {
                // TODO: Lock semaphore, increment counter, unlock
            }
            exit(0);
        }
    }

    // 4. Wait for children
    // TODO

    // 5. Print final counter value
    // TODO

    // 6. Cleanup (close/unlink)
    // TODO

    return 0;
}
