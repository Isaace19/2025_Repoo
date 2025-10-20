#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int jog;
    int num;
    int i;
    int shm;
    // create the semaphore
    sem_t *sem;
    
    if (argc < 6) {
        printf("Usage: %s <sem file> <shm file> <num> <jog> <worker>\n", argv[0]);
        return 1;
    }
    if (1 != sscanf(argv[3], "%d", &num) || num <= 0 || num > 100000) {
        printf("Invalid number for num '%s'\n", argv[3]);
        return 1;
    }
    if (1 != sscanf(argv[4], "%d", &jog) || jog <= 0 || jog > 100000) {
        printf("Invalid number for jog '%s'\n", argv[4]);
        return 1;
    }
    // check, read, and load in semaphore data 
    sem = sem_open(argv[1], O_CREAT | O_EXCL | O_RDWR, 0600, 1);
    if (sem == NULL) {
        perror("sem_open");
        return 1;
    }

    shm = shm_open(argv[2], O_CREAT | O_RDWR | O_EXCL, 0600);
    if (shm == -1) {
        perror("shm_open");
        return 1;
    }
    ftruncate(shm, 4);
    i = 0;
    write(shm, &i, sizeof(i));
    // ensure that children are opened properly after scanning them

    // create heavyweight threads 
    pid_t *pids = malloc(sizeof(pid_t) * num);
    // create via fork
    for (i = 0; i < num ; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // tell the program the num and the jog value that it goes through
            close(shm); // close share memory
            char value[16];
            char sjog[16];
            // every parameter to exec must be a string, so we take the integer values we created
            sprintf(value, "%d", num);
            sprintf(sjog, "%d", jog);
            // integer value to string value
            // TODO: Have to figure out/re-read as to what execl does in terms of argument files
            // first file is the argument file, data, name of program, shared memory file, value, sjog, terminate to NULL
            execl(argv[5], argv[5], argv[1],  argv[2], value, sjog, NULL);
            // feed number and jog value ; then terminate process
            perror("execl");
            return 1;
        }
    }
    // wait for the children process to finish after work is given from command line arguments
    // children quits when work is done via wait pid_t


    // 
    for (i = 0; i < num; i++) {
        waitpid(pids[i], NULL, 0);
    }
    // sem = sem_open();



    // TODO - what does unlink actually do? 
    close(shm);
    // shm_unlink(argv[2]);
    sem_close(sem);
    // sem_unlink(argv[1]); 
    free(pids);
    return 0;
}
