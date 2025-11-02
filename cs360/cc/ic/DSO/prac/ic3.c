// Implement code that will work on worker and thread function
// So in short, create 1 pthread process that should return from our .so file
// the output

// add in a kill signal that is taken from the command line, sending " * " into the terminal
// should kill this program, and output the run time of the thread as in theory this should 
// just keep running 1 + 1in add until signal kills.


#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>

typedef int(* myfunc) (int a, int b);

static void *worker(void *args);


// implement a simple data structure and a structure for synchronization signals
struct list {
    int value;
    struct list *next;
};

struct Data {
    pthread_cond_t cond;
    pthread_mutex_t lock;
    int die;
}

int main () {
    struct Data data;
    int k;
    // create the singular thread process
    pthreads = tids[0];
    pthread_cond_init(&data.cond, NULL);
    pthread_mutex_init(&data.lock, NULL);
    data.die = 0;

    work = malloc(sizeof(struct list));
    work_value = 10;
    work->next = NULL;
    data.work = work;



    myfunc func;
    void *handle;
    int i, j;
    *int gba, gbb;

    handle = dlopen("./plib.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        printf("Error with plib.so\n", dlerror());
        return 1;
    }
    printf("Handle Allocation Success\n");

    // define and create the marshaller.

}




