#include "plugin.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char *fileN = argv[argc - 1];
  int fd = open(fileN, O_RDONLY);
  struct stat st;
  fstat(fd, &st);
  size_t size = (size_t)st.st_size;
  char *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  close(fd);

  void *handle = NULL;
  char *temp;
  char pluginName[100];

  INIT Ifunc;
  SETTING Sfunc;
  TRANSFORM Tfunc;
  int ss = (int)size;

  for (int i = 1; i < argc - 1; i++) {
    temp = argv[i];
    if (strcmp(temp, "-p") == 0 && i + 1 < argc - 1) {
      sprintf(pluginName, "./lib%s.so", argv[i + 1]);
      handle = dlopen(pluginName, RTLD_NOW | RTLD_LOCAL);

      Ifunc = (INIT)dlsym(handle, "init");
      dlerror();
      Sfunc = (SETTING)dlsym(handle, "setting");
      dlerror();
      Tfunc = (TRANSFORM)dlsym(handle, "transform");
      dlerror();
      if (!Tfunc) {
        fprintf(stderr,
                "ERROR: unable to find 'transform' function in plugin %s.\n",
                pluginName);
        dlclose(handle);
        munmap(data, size);
        return (1);
      }
      if (Ifunc) {
        Ifunc();
      }
      i++;

      while (strcmp(argv[i + 1], "-p") != 0 && i + 1 < argc - 1) {
        if (strcmp(argv[i + 1], "-s") == 0 && i + 2 < argc) {
          char *name = strtok(argv[i + 2], "=");
          char *value = strtok(NULL, "\0");

          if (!Sfunc) {
            fprintf(stderr, "ERROR: %s does not take settings.\n", pluginName);
            dlclose(handle);
            munmap(data, size);
            return (1);
          }

          PluginResult s = Sfunc(name, value);
          if (PR_FAILED == s) {
            dlclose(handle);
            munmap(data, size);
            return (1);
          } else if (PR_STOP == s) {
            dlclose(handle);
            munmap(data, size);
            return (0);
          }
          i = i + 2;
        } else {
          break;
        }
      }
      PluginResult t = Tfunc(data, &ss);
      if (t == PR_FAILED) {
        fprintf(stderr, "ERROR: plugin '%s' failed.\n", pluginName);
      } else if (t == PR_STOP) {
        dlclose(handle);
        munmap(data, size);
        return (0);
      }
      dlclose(handle);
    }
  }
  if (ss > (int)size) {
    ss = (int)size;
  }
  write(STDOUT_FILENO, data, ss);
  write(STDOUT_FILENO, "\n", 1);

  munmap(data, size);
  return (0);
}
