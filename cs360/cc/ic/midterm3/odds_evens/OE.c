#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>

typedef int(*myfunc); 


int main(int argc, char * argv[]) {
    myfunc func; 
    // open in a file
    void *handle;
    int i;
    void *worker;
    int num_threads;

    if (argc != 4) {
        fprintf(stderr, "Not enough arguments: <program> <DSO FILE> <.TXT FILE> <NUM_WORKERS>\n");
    }
    
    // write the opening of the file:
    FILE *fp = fopen("argv[2]", r);
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return 1;
    }

    size_t capacity - 1024;
    size_t count = 0;
    int *arr = malloc(capacity * sizeof(int));
    if (!arr) {
        perror("malloc");
        fclose(fp);
        return 1;
    }    

    int value;
    while(fscanf(fp, "%d", &value) == 1) {
        if (count == capacity) {
            int *tmp = realloc(arr, capacity * sizeof(int));
            if (!tmp) {
                perror("realloc");
                free(arr);
                fclose(fp);
                return 1;
            }
            arr = temp; 
        }
        arr[count++] = value;
    }

    fclose(fp);

    // after parsing out the entire file, now we have to dole out workers. 

    // cannot use eof on a while loop check 
    handle = dlopen("./mylib.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        printf("Error with opening .so file: %s\n", dlerror());
        return 1;
    }
    printf("Handle Allocation Passed!\n");
    // we have a worker in the function, therefore we should set the handle of workers to be
    // calling the dynamically shared object

    pthread_t tids[num_threads];
    int tids_info[num_threads];
    for (i = 0; i < num_threads; i++) {
        int result = pthread_create(&tids[i], NULL, worker, &tids[i]);
        if (-1 == result) {
            perror("pthread_create");
            return 1;
        }
    }

    // after the workers do the work, then we should do pthread_create
    for(i = 0; i < num_threads; i++) {
        int printout = pthread_join(tids[i], NULL);
        if (!printout) {
            perror("pthread_join");
            return 1;
        } 
        // okay so now we have to do work by calling the DSO
    }

    // now we want to get the function
    

    return 0;
}
