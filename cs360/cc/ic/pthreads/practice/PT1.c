#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Do not change this, this is the worker you'll need to
// call when you create your threads.
void *worker(void *arg) {
  int *v = (int *)arg;
  *v = 1234;
  return NULL;
};

int main(int argc, char *argv[]) {
  int num_threads;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <num threads>\n", argv[0]);
    return 1;
  }
  if (1 != sscanf(argv[1], "%d", &num_threads) || num_threads <= 0 ||
      num_threads > 1000) {
    fprintf(stderr, "Invalid number of threads: '%s'\n", argv[1]);
    return 1;
  }
  pthread_t tcb[num_threads]; // thread control block
  int thread_ids[num_threads];
  void *status;

  // goal is to create 500 pthreads.

  int i;
  for (i = 0; i < num_threads; i += 1) {
    thread_ids[i] = i;
    int result = pthread_create(&tcb[i], NULL, worker, &thread_ids[i]);
    if (result != 0) {
      perror("pthread_create");
      exit(1);
    }
  }
  // the inner code block in the loop creates a singular thread, then we do this
  // for the amount of numthreads, anywhere form 1 to 999 according to our
  // above error check of argv[1];

  // wait for th thread to exit, then exit
  // does threading have to loop over the amount of threads as well?

  // we have to say thread_ids because
  // we are trying to print out all the ids and not the raw memory address as
  // pointed to by tcb.

  // in short, tcb creates the pointer threads, while thread_ids stores the id
  // values in each thread.
  for (i = 0; i < num_threads; i += 1) {
    if (pthread_join(tcb[i], &status) != 0) {
      perror("pthread_join");
      exit(1);
    }
    printf("%d", thread_ids[i]);
    printf("\n");
  }

  return 0;
}
