#include <dlfcn.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ### ORIGINAL SUBMISSION FOR ZYBOOK THAT WORKED 100/100 ### */
struct Work {
  int test_val;
  int test_target;
  bool result;
};

typedef void *(*WORKERFUNC)(void *arg);
typedef void (*REPORTERFUNC)(const struct Work *w);

int main(int argc, char *argv[]) {
  int target;
  WORKERFUNC wf;
  REPORTERFUNC rf;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <target> <dso>\n", argv[0]);
    return 1;
  }
  if (1 != sscanf(argv[1], "%d", &target)) {
    fprintf(stderr, "Invalid target '%s'\n", argv[1]);
    return 1;
  }
  // target holds argv[1]
  target += 100;

  char libpath[256] = {0};
  const char *stem = argv[2];

  // do the command line parsing to add in lib or .so as necessary
  if (strncmp(stem, "./", 2) != 0) {
    strcat(libpath, "./");
  }
  if (strncmp(stem, "lib", 3) != 0) {
    strcat(libpath, "lib");
  }
  strcat(libpath, stem);
  if (!strstr(libpath, ".so")) {
    strcat(libpath, ".so");
  }

  // handle opening the handle and the DSO
  void *handle;
  handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    printf("Error with opening the .so file: '%s'\n", dlerror());
    return 1;
  }

  wf = dlsym(handle, "dso_worker_func");
  if (NULL == wf) {
    fprintf(stderr, "There is an error extracting the symbol\n");
    return 1;
  }

  rf = dlsym(handle, "dso_reporter_func");
  if (NULL == rf) {
    fprintf(stderr, "There is an error extracting the symbol\n");
    return 1;
  }

  // int threads;
  //  sscanf(argv[1], "%d", &threads);

  int i;
  pthread_t tids[target];
  // int tids_info[target];

  // pthread_t *tids = calloc(target, sizeof(pthread_t));
  // array of work structures with "target" number of work structures
  struct Work *work = calloc(target, sizeof(struct Work));

  for (i = 0; i < target; i += 1) {
    work[i].test_val = i + 1;
    work[i].test_target = target;

    int process = pthread_create(&tids[i], NULL, wf, &work[i]);
    if (-1 == process) {
      perror("pthread_create");
      return 1;
    }
  }
  void *status;
  // after the workers are done, then we join
  for (i = target - 1; i >= 0; i -= 1) {
    // pthread_join(tids[i], NULL) also works - Man Pages.
    if (pthread_join(tids[i], &status) != 0) {
      perror("pthread_join");
      return 1;
    }
    // loop over the workers and output in reverse:
    rf(&work[i]);
  }

  // free(tids);
  free(work);
  dlclose(handle);
  return 0;
}
