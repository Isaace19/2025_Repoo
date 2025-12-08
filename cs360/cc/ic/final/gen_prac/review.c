#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <string.h>

typedef struct Data {
    int param[3]; // can hold three params inside of this from data.param[] access
} Data;


typedef struct Work {
    double x;
    double y;
    double z;
} Work;


// workers parse the lines out or do something 
typdef struct {
    char name[50];
    int id;
} Worker;


int main(int argc, char *argv[]) {
    // write in the basics of command line parsing - being able to handle 
    Data data;
    Work work;
    
    // define worker function
    Worker *workers;
    int num_workers = 0;
    
    
    if (scanf("%d %d %d", &data.parm[0], &data.param[2], &data.param[2]) != 3) {
        printf("Bad input:\n");
        return 1;
    }
    
    // verify joe
    printf("The Following has been outputed: %d, %d %d\n, data.param[0], data.param[1], data.param[2]");
    
    
    while (scanf("%lf %lf %lf", work.x, work.y, work.z) == 3) {
        
    }
    
    // be able to read and write files using buffered stream
    FILE* fptr;
    char* buffer;
    
    // in case argv that works with whatever argv index from command line
    const char *filename = argv[4];
    fptr = fopen(filename, "rw"); 
    while (fptr == NULL) {
        printf("Error in reading in the file");
        return 1;
    }   
    // dole out the workers
    workers = (Worker *)malloc(sizeof(worker));
    if (workers == NULL) {
        perror("malloc");
        fclose(fptr);
        return 1;[B
    }
    
    
    // read file line by line and have workers parse that data
    char lb[256];
    while (fgets (lb, sizeof(lb), fptr) != NULL) {
        
    }
    
    close (fptr);
    return 0;
}



// secondary work of working with file buffering to manage files
int main(int argc, char *argv[]) {
    FILE *fd;
    char buffer[256];
    
    char *filename = argv[0];
    
    fd = fopen(filename, "rw");
    if (NULL == fd) {
        fprintf(stderr, "File failed to open.\n");
        return 1;
    }
    
    
}
