#include "plugin.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// implement the loader function here to read in the command line
// for the plugin files

// the last arg from command line must be a file

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Not enough arguments. Usage: %s [plugins.so]\n", argv[0]);
    return 1;
  }

  // handle the command line arguments
  char libpath[256] = {0};
  const char *stem = argv[2];

  // when given the -p flag, so this should be argv[1]
  // apply the ./ to the file for absolute path reasons
  // so prepend ./lib and append .so
  // optionally we have a settings flag -s

  // create two const c style strings to load into settings
  // handle as a while loop checking for the flag "-"
  int *flag = argv;

  while (1) {
    switch (flag) {
    case "-p":
      strcat(libpath, "./lib");
      strcat(libpath, stem);
      strcat(libpath, ".so");
    case "-s":
      // followed by a name=value seequnce
      // find the arguments immediately after the flag
      char *result = malloc(name + value + 2); // null term + =
      const size_t len1 = strlen(name);
      const size_t len2 = strlen(value);
      memcpy(result, name, len1);
      memcpy(result + len1, "=", len2 + 2);
      return result;
    }
  }

  void *handle;
  handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Erorr with opening the .so file: %s\n", dlerror());
    return 1;
  }
  printf("Handle Allocation has passed\n");

  // using mmap because we need to map the inputted file into memory?
  int shmfd = shm_open(handle, O_RDONLY);
  if (shmfd == -1) {
    perror("shm_open");
    return 1;
  }
  ftruncate(shmfd, sizeof(int));
  void *map =
      mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  int *output = (int *)map;
  printf("Shared memory creation successful\n");
  // after opening the file successfully and creating shared memory
  // we look for hte proper signals

  struct PluginFunction *PF = (struct PluginFunction *)dlsym(handle, "INIT");
  if (!PF) {
    printf("Symbol INIT not found in the handle table\n");
    return 1;
  }
  // is it possible to do multiple dlsym lookups using the same handle?
  PF = (struct PluginFunction *)dlysm(handle, "SETTING");
  if (!PF) {
    printf("Symbol SETTING not found in the handle table\n");
    return 1;
  }
  PF = (struct PluginFunctio *)dlsym(handle, "TRANSFORM");
  if (!PF) {
    printf("Symbol TRANSFORM not found int the handle table\n");
    return 1;
  }

  munmap(map, sizeof(int));
  close(shmfd);
  dlclose(handle);
  return 0;
}
