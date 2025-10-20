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

int main(int argc, char *argv[])
{
    // declare constants
    sem_t *sem;
    mpd_t mq;
    int shm;
    Data data;
    unsigned int prio;
    ssize_t bytes;


    if (argc < 4) {
        printf("Usage: %s <mqueue> <sem> <shm>\n", argv[0]);
        return 1;
    }
/*
1. open the message queue, sempahore, and shared memory
    -> do not add in O_CREAT flag when reading them in
2. Set signal to quit at -1 when all of the messages in thread x,y,z are 
*/
    mq = mq_open(argv[1], O_EXCL | O_WRONLY);
    if (mq < 0){
        perror("mq_open");
        return 1;
    }

    // check, read, and load in semaphore data 
    sem = sem_open(argv[2], O_EXCL | O_RDWR);
    if (sem == NULL) {
        perror("sem_open");
        return 1;
    }

    shm = shm_open(argv[3],  O_RDWR | O_EXCL);
    if (shm == -1) {
        perror("shm_open");
        return 1;
    }

    // pop messages until x,y,z are equal to -1

    int i;
    // need to do a map in order to read i the data and manipulate it. create a memmap using the shared memory file
    mem = memmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    const DATA terminate_signal = {-1, -1, -1};


    // working on the receiving end loop
    while(1){
        DATA message;
        mq_receive(mq, )
        printf("This loop should work\n");
    }

    close(mq);
    close(shm);
    return 0;
}

