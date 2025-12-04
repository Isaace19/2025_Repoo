#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdlib.h>

struct Work {
    int test_val;
    int test_target;
    bool result;
};

typedef void*(*WORKERFUNC)(void *arg);
typedef void(*REPORTERFUNC)(const struct Work *w);


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

  char *input = argv[2];
  char core[512] = {0};
  char *clean = core;
  size_t len = strlen(input);

  // step 1: strip leading "lib" only if it is the prefix
  if (strncmp(input, "lib", 3) == 0) {
    input += 3;
  }

  // copy everything into empty core buffer
  while (*knput) {
    *clean++ = *input++;
  }
  *clean = '\0';

  // strip out the trailing .so only if it is actually the suffix
  len = strlen(core);
  if (len >= 3 && strcmp (core + len  - 3, ".so") == 0) {
    core[len - 3] = '\0';
  }

  // build the final correct string always:
  char result[4096] = {0};
  snprintf(result, sizeof(result), "./lib%s.so", core);
  printf("%s\n", result);

  // handle opening the handle and the DSO
  void *handle;
  handle = dlopen(result, RTLD_LAZY | RTLD_GLOBAL);
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

  pthread_t tids[target];
  int tids_info[target];

  // pthread_t *tids = calloc(target, sizeof(pthread_t));
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
  for (i = 0; i < target; i += 1) {
    if (pthread_join(tids[i], &status) != 0) {
      perror("pthread_join");
      return 1;
    }
  }

  for (i = 0; i < target; i += 1) {
    rf(&work[i]);
  }

  // free(tids);
  free(work);
  dlclose(handle);
  return 0;
}
