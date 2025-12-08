#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

// shared mem implementation

typedef struct {
    int foo;
} Foo;


int main(int argc, char* argv[]) {
    int *fd;
    char buffer[256];
    
    char *filename = argv[0];
    
    int shm = shm_open(filename, O_RDWR | O_EXECL | O_CREAT, 06444);
    if (NULL == shm) {
        perror("shm_open");
        return 1;
    }
    
    ftruncate(shm, sizeof(struct Foo));
    // truncate then mmap it to have the shared memory
    struct foo *ptr = mmap(NULL, sizeof(struct Foo), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (ptr == MAP_FAILED) {
        fprintf(stderr, "Failed to allocate SHARED memory");
        return 1;
    }

    // we need to free shared memory
    munmap(ptr, sizeof(struct Foo));
    close(shm);
    return 0;
}