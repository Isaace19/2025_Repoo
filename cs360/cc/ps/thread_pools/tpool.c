#include "tpool.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Write your five (5) functions here
// All are prototyped in tpool.h

// data that all thread jobs would have (have to verify if this is right)

struct threadpool {
  // control of the threads
  int next; // index of job
  int done;
  int total; // total jobs in batch
  uint64_t *result;
  const int *input;

  // threads
  int nthreads;
  bool die;
  pthread_t *tids;

  // func pointer
  Executor executor;

  // thread synchronization
  pthread_cond_t job_available;
  pthread_cond_t all_done;
  pthread_mutex_t lock;
};

static void *worker_thread(void *arg) {
  struct threadpool *tp = arg;

  for (;;) {
    pthread_mutex_lock(&tp->lock);

    // Wait while no work is available and not shutting down
    while (!tp->die && tp->next >= tp->total)
      pthread_cond_wait(&tp->job_available, &tp->lock);

    // Shutdown check
    if (tp->die) {
      pthread_mutex_unlock(&tp->lock);
      break;
    }

    // Take one job index safely
    int jid = tp->next++;
    int val = tp->input[jid];
    Executor exec = tp->executor;
    pthread_mutex_unlock(&tp->lock);

    // Execute job outside of lock
    uint64_t res = exec(val);

    // Write result and update done counter
    pthread_mutex_lock(&tp->lock);
    tp->result[jid] = res;
    tp->done++;

    // If all jobs finished, wake up main thread
    if (tp->done == tp->total)
      pthread_cond_signal(&tp->all_done);

    pthread_mutex_unlock(&tp->lock);
  }

  return NULL;
}

void *thread_pool_open(int num_threads) {
  if (num_threads < 1 || num_threads > 32)
    return NULL;

  struct threadpool *tp = calloc(1, sizeof(*tp));
  if (!tp)
    return NULL;

  tp->nthreads = num_threads;
  tp->die = false;
  tp->tids = calloc(num_threads, sizeof(pthread_t));
  if (!tp->tids) {
    free(tp);
    return NULL;
  }

  // Initialize control fields
  tp->next = 0;
  tp->done = 0;
  tp->total = 0;
  tp->result = NULL;
  tp->input = NULL;
  tp->executor = NULL;

  // Initialize synchronization primitives
  pthread_mutex_init(&tp->lock, NULL);
  pthread_cond_init(&tp->job_available, NULL);
  pthread_cond_init(&tp->all_done, NULL);

  // Spawn worker threads
  for (int i = 0; i < num_threads; i++) {
    if (pthread_create(&tp->tids[i], NULL, worker_thread, tp) != 0) {
      tp->die = true;

      // Clean up partially created threads
      for (int j = 0; j < i; j++)
        pthread_join(tp->tids[j], NULL);

      pthread_mutex_destroy(&tp->lock);
      pthread_cond_destroy(&tp->job_available);
      pthread_cond_destroy(&tp->all_done);
      free(tp->tids);
      free(tp);
      return NULL;
    }
  }
  return tp; // handle returned to user
}

uint64_t *thread_pool_execute(void *handle, const int *work_vals, int num_work,
                              const Executor executor) {
  uint64_t *results = calloc(num_work, sizeof(uint64_t));
  struct threadpool *tp = (struct threadpool *)handle;

  pthread_mutex_lock(&tp->lock);

  tp->result = results;
  tp->input = work_vals;
  tp->executor = executor;
  tp->next = 0;
  tp->done = 0;
  tp->total = num_work;

  pthread_cond_broadcast(&tp->job_available);

  while (tp->done < tp->total) {
    pthread_cond_wait(&tp->all_done, &tp->lock);
  }
  pthread_mutex_unlock(&tp->lock);
  return results;
}

void thread_pool_close(void *handle) {
  if (handle == NULL) {
    return;
  }

  struct threadpool *tp = handle;
  pthread_mutex_lock(&tp->lock);
  tp->die = true;
  pthread_cond_broadcast(&tp->job_available);
  pthread_mutex_unlock(&tp->lock);
  int i;
  for (i = 0; i < tp->nthreads; i++) {
    pthread_join(tp->tids[i], NULL);
  }

  pthread_mutex_destroy(&tp->lock);
  pthread_cond_destroy(&tp->job_available);
  pthread_cond_destroy(&tp->all_done);

  free(tp->tids);
  free(tp);
}

// write the executors

// 32 bit hashing
uint64_t hash32(int fd) {
  uint32_t fnv = 2166136261;
  uint32_t hash = 16777619;
  uint32_t digest = fnv;
  // create a loop that will perform the xor and mult on the two values in a
  // buffer
  unsigned char buff[4096];
  ssize_t swap;

  while ((swap = read(fd, buff, sizeof(buff))) > 0) {
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

  while ((swap = read(fd, buff, sizeof(buff))) > 0) {
    for (ssize_t i = 0; i < swap; i++) {
      digest ^= buff[i];
      digest *= hash;
    }
  }
  return (digest);
}