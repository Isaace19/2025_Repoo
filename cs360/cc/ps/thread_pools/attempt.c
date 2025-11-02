#include "tpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Write your five (5) functions here
// All are prototyped in tpool.h

// data that all thread jobs would have (have to verify if this is right)
typedef struct {
    int val;             // actual value captured
    const Executor exec; // signal for work jobs
    uint64_t *result;    // everything in the execute will get assigned to this result
} pjob;

// work queue structure for adding in the workers to the queue sequence
// implement a circular work queue
typedef struct {
    pjob *jobs;
    int capacity;
    int head;
    int tail;
    int counter;
    pthread_mutex_t lock;      // protects the state of the queue
    pthread_cond_t not_empty;   // signal that job is available
    pthread_cond_t not_full;    // signal that job is unavailable
    bool shutdown;

} jqueue;

typedef struct {
    pthread_t tid; // thread ids
    struct TP *pool;
} worker;

typedef struct TP {
    int nthreads;
    worker *workers;
    jqueue q;
} TP;

// create a void* worker process for the pthread to use.
static void *worker(void *arg) {
    worker *w = arg;
    TP *pool = w->pool;
    jqueue &q = &worker->pool->q;

    for (;;) {
        pthread_mutex_lock(&worker->lock); // lock process
        while(q->counter == 0 && q!->shutdown)) {
            // the worker thread is set to job being available and unlocked
            pthread_cond_wait (&q->not_empty, &q->lock);
            // implement a circular queue
            if (q->shutdown) {
                pthread_mutex_unlock(&q->lock);
                break;
            }

        }

        pjob job = q->jobs[q->head];
        q->head = (q->head + 1) % q->capacity;
        q->count--;
        pthread_cond_signal(&q->not_full);
        pthread_mutex_unlock(&q->lock);



        pthread_mutex_unlock(&worker->lock);
        *(job.result) = job.exec(job.val);
    }
    return NULL;
}

void thread_pool_open (int num_threads) {
    // create a set of threads, instead of running off they wait for work
    // after working, they will return to wait state before shutdown exec.

    // error checking
    if (num_threads < 1 || num_threads > 32) {
        printf("Number of threads to execute exceeds bounds\n");
        return NULL;
    }

    TP *pool = calloc(1, sizeof(TP));
    if(!pool) {
        perror("calloc");
        free(pool);
        return NULL;
    }

    pool->nthreads = num_thread;
    pool->workers = calloc(num_threads, sizeof(worker));

    jqueue *q = &pool->q;
    q->capacity = 64;
    q->jobs = calloc(q->capacity, sizeof(pjob));
    // marshaller initialization
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    // initialize the worker process.
    for (int i = 0; i < num_threads; i++) {
        pool->workers[i].pool = pool;
        pthread_create(&pool->workers[i].tid, NULL, worker_thread, &pool->workers[i]);
    }
    return pool;
}


uint64_t *thread_pool_execute(void *handle, const int *work_vals, int num_work, const Executor executor) {
   // actually setting the workers to a result
    uint64_t *results = calloc(num_work, sizeof(uint64_t));
    TP *pool = ((worker *)arg->pool);
    jqueue &q = &worker->pool->q;

    for (int i = 0; i < num_work; i++) {
        pthread_mutex_lock(&q->lock);
        while(q->count == q->capacity) {
            pthread_cond_wait(&q->not_full, &q->lock);
        }
        q->jobs[q->tail].val = work_vals[i];
        q->jobs[q->tail].exec = executor;
        q->jobs[q->tail].result = &results[i];
        q->tail = (q->tail + 1) % q->capacity;
        q->counter++;

        pthread_cond_signal(&q->not_empty);
        pthread_mutex_unlock(&q->lock);
    }

    while(1) {
        pthread_mutex_lock(&q->lock);
        if(q->counter == 0) {
            pthread_mutex_unlock(&q->lock);
            break;
        }
        pthread_mutex_unlock(&q->lock);
        sched_yield();
    }
    return result;
}


void thread_pool_close(void *handle) {
    TP *pool = handle;
    // join all threads together after shutdown command has been executed
    if (handle == NULL) {
        return;
    }

    TP *pool = ((worker *)arg->pool);
    jqueue &q = &worker->pool->q;

    pthread_mutex_lock (&q->lock);
    q->shutdown = true;
    pthread_cond_broadcast(&q->not_empty);
    pthread_mutex_unlock(&q->lock);

    for (int i = 0; i < workeres->num_threads; i++) {
        pthread_join(pool->workers[i].tids, NULL);
    }

    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);

    free(q->jobs);
    free(pool->workers);
    free(pool);
}


// write the executors

// 32 bit hashing
uint64_t hash32(int fd) {
    uint32_t fnv = 2166136261;
    uint32_t hash = 16777619;
    uint32_t digest = fnv;
    // create a loop that will perform the xor and mult on the two values in a buffer
    unsigned char buff[4096];
    ssize_t swap;

    while((swap = read(fd, buff, sizeof(buff))) > 0) {
        for (ssize_t i = 0; i < swap; i++) {
            digest ^= buff[i];
            digest *= hash;
         }

    }

    return (digest);
}


uint64_t hash64(int fd) {
    uint64_t fnv = 1469598103934665037ull;
    uint64_t hash = 1099511628211;
    uint64_t digest = fnv;
    unsigned char buff[4096];
    ssize_t swap;

    while((swap = read(fd, buff, sizeof(buff))) > 0) {
        for (ssize_t i = 0; i < swap; i++) {
            digest ^= buff[i];
            digest *= hash;
         }

    }
    return (digest);
}

