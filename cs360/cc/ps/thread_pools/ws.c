#include "tpool.h"
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

struct Worker {
  int totalW;
  int done;
  int at;
  
  uint64_t *results;
  const int *data;
  
  int numThreads;
  bool die;
  pthread_t *tids;
  Executor executor;

  pthread_cond_t stop;
  pthread_cond_t go;
  pthread_mutex_t lock;
};

static void *worker(void *arg)  //need to check this function. 
{
  struct Worker *w = (struct Worker *)arg; 
  for(;;)
  { 
    pthread_mutex_lock(&w->lock);
    while(!w->die && (w->at >= w->totalW))
    {
      pthread_cond_wait( &w->go , &w->lock );
    }
    if(w->die)
    {
      pthread_mutex_unlock(&w->lock);
      break;
    }

    int temp = w->at++;
    pthread_mutex_unlock(&w->lock);
    uint64_t r = w->executor(w->data[temp]);
    pthread_mutex_lock(&w->lock);
    w->results[temp] = r;
    w->done++;

    if(w->done == w->totalW)
    {
      pthread_cond_signal(&w->stop);
    }
    pthread_mutex_unlock(&w->lock);
  }
  return (NULL);
}

// Write your five (5) functions here
// All are prototyped in tpool.h 
void *thread_pool_open(int num_threads)
{
  if(num_threads < 1 || num_threads > 32)
  {
    return (NULL);
  }
  struct Worker *workers = calloc( 1 , sizeof(*workers) );
  void *handle;
  int i;
  pthread_mutex_init(&workers->lock, NULL);
  pthread_cond_init(&workers->go, NULL);
  pthread_cond_init(&workers->stop, NULL);

  workers->numThreads = num_threads;
  workers->die = false;
  workers->tids = calloc(num_threads, sizeof(pthread_t));
  workers->at = 0;
  workers->done = 0;
  workers->totalW = 0;
  workers->results = NULL;
  workers->data = NULL;
  workers->executor = NULL;


  for(i = 0 ; i < num_threads ; i++)
  {
    pthread_create( &workers->tids[i] , NULL , worker , workers);
  }
  handle = workers;
  return (handle);
}





uint64_t *thread_pool_execute(void *handle, const int *work_vals, int num_work, const Executor executor)
{
  uint64_t *results = calloc(num_work, sizeof(uint64_t));
  struct Worker *w = (struct Worker *)handle;

  pthread_mutex_lock(&w->lock);

  w->results = results;
  w->data = work_vals;
  w->executor = executor;
  w->at = 0;
  w->done = 0;
  w->totalW = num_work;

  pthread_cond_broadcast(&w->go);

  while(w->done < w->totalW)
  {
    pthread_cond_wait(&w->stop, &w->lock);
  }
  pthread_mutex_unlock(&w->lock);

  return (results);
}




void thread_pool_close(void *handle)
{
  if(handle == NULL)
  {
    return;
  }
  struct Worker *workers = handle;

  pthread_mutex_lock(&workers->lock);
  workers->die = true;
  pthread_cond_broadcast(&workers->go);
  pthread_mutex_unlock(&workers->lock);

  for(int i = 0; i < workers->numThreads; i++)
  {
    pthread_join(workers->tids[i], NULL);
  }

  pthread_mutex_destroy(&workers->lock);
  pthread_cond_destroy(&workers->go);
  pthread_cond_destroy(&workers->stop);

  free(workers->tids);
  free(workers);
}

// Executors   
uint64_t hash32(int fd)
{ 
  uint32_t a = 2166136261;
  uint32_t b = 16777619;
  uint32_t digest = a;
  unsigned char buffer[4096];
  ssize_t temp;
  while((temp = read(fd, buffer, sizeof(buffer))) > 0)
  {
    for(ssize_t i = 0; i < temp; i++)
    {
      digest ^= buffer[i];
      digest *= b;
    }
  }
  return (digest);
}




uint64_t hash64(int fd)
{
  uint64_t a = 14695981039346656037ull;
  uint64_t b = 1099511628211;
  uint64_t digest = a;
  unsigned char buffer[4096];
  ssize_t temp;
  while((temp = read(fd, buffer, sizeof(buffer))) > 0)
  {
    for(ssize_t i = 0; i < temp; i++)
    {
      digest ^= buffer[i];
      digest *= b;
    }
  }
  return (digest); 
}
