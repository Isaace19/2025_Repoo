#include "plugin.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// implement the loader function here to read in the command line
// for the plugin files

// the last arg from command line must be a file

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Not enough arguments. Usage: %s -p plugin [-s key=value]\n",
            argv[0]);
    return 1;
  }

  // handle the command line arguments
  const char *filename = argv[argc - 1];
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }
  struct stat st;
  fstat(fd, &st);
  size_t size = st.st_size;

  void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    perror("mmap");
    return 1;
  }
  close(fd);

  char libpath[256] = {0};
  // char settings[256] = {0};
  int opt;
  char name[128], value[128];
  int n = (int)size;
  // when given the -p flag, so this should be argv[1]
  // apply the ./ to the file for absolute path reasons
  // so prepend ./lib and append .so
  // optionally we have a settings flag -s

  // create two const c style strings to load into settings
  // handle as a while loop checking for the flag "-"
  optind = 1;
  while ((opt = getopt(argc, argv, "p:s:")) != -1) {
    switch (opt) {
    case 'p': {
      snprintf(libpath, sizeof(libpath), "./lib%s.so", optarg);
      void *h = dlopen(libpath, RTLD_NOW);
      if (!h) {
        fprintf(stderr, "%s\n", dlerror());
        munmap(data, size);
        return 1;
      }

      void (*init)(void) = dlsym(h, "init");
      void (*setting)(const char *, const char *) = dlsym(h, "setting");
      void (*transform)(char *, size_t) = dlsym(h, "transform");
      if (init)
        init();

      // process subsequent -s args
      int s_index = optind;
      while (s_index < argc && strcmp(argv[s_index], "-p") != 0 &&
             s_index != argc - 1) {
        if (strcmp(argv[s_index], "-s") == 0 && s_index + 1 < argc - 1) {
          char *eq = strchr(argv[s_index + 1], '=');
          if (eq) {
            size_t klen = eq - argv[s_index + 1];
            strncpy(name, argv[s_index + 1], klen);
            name[klen] = '\0';
            strncpy(value, eq + 1, sizeof(value) - 1);
          } else {
            strncpy(name, argv[s_index + 1], sizeof(name) - 1);
            value[0] = '\0';
          }
          if (setting)
            setting(name, value);
          else
            fprintf(stderr, "ERROR: %s does not take settings.\n", libpath);
          s_index += 2;
        } else
          break;
      }
      optind = s_index;

      if (!transform) {
        fprintf(stderr,
                "ERROR: unable to find 'transform' function in plugin %s.\n",
                libpath);
        dlclose(h);
        munmap(data, size);
        return 1;
      }

      transform(data, n);
      dlclose(h);
      break;
    }
    }
  }
  if (n > (int)size) {
    n = (int)size;
  }
  write(STDOUT_FILENO, data, size);
  write(STDOUT_FILENO, "\n", 1);
  munmap(data, size);
  return 0;
}