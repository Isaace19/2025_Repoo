#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

struct Data {
    int value;
    bool is_prime;
};

struct Result {
    struct Data data;
    struct Result *next;
};

struct ResultList {
    struct Result *head;
};


// bounded circular buffer
struct WorkQueue {
    int size;// # of items currently in the queue
    int capacity;//max amounts of items
    int at;// index of the oldest data at the front
    int *data;// actual storage
};

struct Worker {
    bool die;
    pthread_t tid;
    // Add other stuff here
    
    // mutex locking protects both queue and result list
    // not_empty - workers wait on this queue when empty
    // not_full - marshaller waits on this when queue is full
    pthread_cond_t *not_empty;
    pthread_cond_t *not_full;

    pthread_mutex_t *lock;
    struct WorkQueue *queue;
    struct ResultList *results;
};

static void *worker(void *arg);
static int  wq_pop(struct WorkQueue *wq);
static bool wq_push(struct WorkQueue *wq, int value);
static bool wq_full(const struct WorkQueue *wq);
static bool wq_empty(const struct WorkQueue *wq);
static bool awful_is_prime(int value);

int main(int argc, char *argv[]) 
{
    int i;
    int start;
    int end;
    int num_workers;
    int work_queue_size;

    /*
    Command Line parsing section
    
    Scanf ensures that there are 4 arguments, it converts them to 
    integers without the bounds checking. 
    
    Using start and end clamping since prime checking is slow
    */

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number of workers> <work queue size> <start prime> <end prime>\n", argv[0]);
        return 1;
    }

    if (1 != sscanf(argv[1], "%d", &num_workers) || num_workers <= 0 || num_workers > 10000) {
        fprintf(stderr, "Unable to convert number of threads '%s'.\n", argv[1]);
        return 1;
    }
    if (1 != sscanf(argv[2], "%d", &work_queue_size)) {
        fprintf(stderr, "Unable to scan work queue size '%s'\n", argv[2]);
        return 1;
    }
    if (1 != sscanf(argv[3], "%d", &start)) {
        fprintf(stderr, "Unable to convert starting prime '%s'\n", argv[3]);
        return 1;
    }
    if (1 != sscanf(argv[4], "%d", &end)) {
        fprintf(stderr, "Unable to convert ending prime '%s'\n", argv[4]);
        return 1;
    }
    if (start < 2) {
        start = 2;
    }
    if (end > 10000000) {
        end = 10000000;
    }
    
    
    /*
    Marshaller Explanation 
    init the data structures and synchronization objects that all threads will use.    
    
    pthread lock, not_full, not_empty set to the PTHREAD_MUTEX_INITALIZER
    lock - protect the workqueue and resultlist from accessing
    not_empty - workers wait here when teh queue is empty marshaller signals when it adds work
    not_full - marshaller waits here when queue is full; workers signal when they remove work 
    
    initialize the shared buffer struct WorkQueue queue = 0 so everything is 0
    calloc for 0 contig. mem block
    
    set the struct ResultList results = {.head = NULL};
    worker thread metadata is calloc'd 
    
    the marshaller runs a while start <= end loops
    lock the shared mutex, no exclusive access to queue
    if queue is full, wait on not_full
    releases lock and blocks, when the worker finsihes and signals not_full, this thread wakes up and 
    re-locks before continuing
    
    push as much work as possible, then signal not_empty to wake one waiting worker
    
    
    
    */    

    // Write the marshaller here
    pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
    pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    struct Worker *workers = calloc(num_workers, sizeof(*workers));
    struct ResultList results = {.head = NULL};
    struct WorkQueue queue = {0};
    queue.data = calloc((work_queue_size), sizeof(*queue.data));

    queue.capacity = work_queue_size;


    /*
    pthread work creation
    each worker gets pointer to shared resources
    same queue, same result list, same lock, same CVs
    die = false keeps the workers running
    */

    for (i = 0; i < num_workers; i+=1) {
        // since values are calloc, they need to be set to value
        workers[i].queue = &queue;
        workers[i].results = &results;
        workers[i].not_full = &not_full;
        workers[i].not_empty = &not_empty;
        workers[i].lock = &lock;
        workers[i].die = false;
        pthread_create(&workers[i].tid, NULL, worker, workers + i);
    }


    /*
    If Queue is full, the main thread waits on not_full
    while waiting, it unlocks the lock sig, so workers are consuming
    
    once space is available, push as many as can fit
    after unlocking, signal one worker that the work is available
    */

    while (start <= end) {
        pthread_mutex_lock(&lock);
        while(wq_full(&queue)){
            pthread_cond_wait(&not_full, &lock);
        }
        while(!wq_full(&queue) && start <= end) {
            wq_push(&queue, start++);
        }
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&not_empty);
    }

    while(!wq_empty(&queue)) {
        usleep(100000);
    }
    // queue is empty -> send a die signal
    for (i = 0; i < num_workers; i+=1) {
        workers[i].die = true;
    }
    pthread_cond_broadcast(&not_empty);
    for(i = 0; i < num_workers; i += 1) {
        pthread_join(workers[i].tid, NULL);
    }
    struct Result *r, *n;
    for (r = results.head; r != NULL; r = n) {
        n = r->next;
        printf("%-3d is%s prime.\n", r->data.value, r->data.is_prime ? "" : " NOT");
        free(r);
    }

    free(workers);
    free(queue.data);


    return 0;
}


/*
Work function pulling and processing work
Worker sleeps when no work and not dying releases the lock while waiting
Pop values safely - only one thread can be here at a time
Immediately unlocks - lets other threads proceed
Prime computation happens without holding lock, avoids blocking the other
Signal not_full so marshaller can push more work
Then re-lock to safely update the shared linked list
*/

void *worker(void *arg)
{
    struct Worker *w = (struct Worker *)arg;
    struct Result *r; 
    int v;
    // Write the worker here
    
    pthread_mutex_lock(w->lock);
    for(;;) {
        // if no work, can't work
        while (!w->die && wq_empty(w->queue)) {
            // unlocks and waits until a new signal relocks it
            pthread_cond_wait(w->not_empty, w->lock);
        }
        // either die sig. or we have a queue with elms.
        if (w->die) {
            pthread_mutex_unlock(w->lock);
            break;
        }
        // work queue implementation
        v = wq_pop(w->queue);
        pthread_mutex_unlock(w->lock);
        // queue is not full
        pthread_cond_signal(w->not_full);
        // linked list implementation
        r = malloc(sizeof(*r));
        r->data.value = v; // r takes on data and the value set to it
        // calculate prime
        r->data.is_prime = awful_is_prime(v);
        // push to linked list
        pthread_mutex_lock(w->lock);
        r->next = w->results->head;
        w->results->head = r;
    }
    return NULL;
}

static int wq_pop(struct WorkQueue *wq)
{
    int ret;
    if (wq_empty(wq)) {
        return -1;
    }
    ret = wq->data[wq->at];
    wq->at = (wq->at + 1) % wq->capacity;
    wq->size -= 1;
    return ret;
}
static bool wq_push(struct WorkQueue *wq, int value)
{
    if (wq_full(wq)) {
        return false;
    }
    wq->data[(wq->at + wq->size) % wq->capacity] = value;
    wq->size += 1;
    return true;
}
static bool wq_full(const struct WorkQueue *wq)
{
    return wq->size >= wq->capacity;
}
static bool wq_empty(const struct WorkQueue *wq)
{
    return wq->size == 0;
}

static bool awful_is_prime(int value)
{
    int i;
    bool is_prime;

    if (value < 2) {
        return false;
    }

    is_prime = true;
    for (i = 2;i < value;i+=1) {
        if (0 == (value % i)) {
            is_prime = false;
        }
    }

    return is_prime;
}

