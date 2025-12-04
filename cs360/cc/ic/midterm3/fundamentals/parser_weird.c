#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[8192];
// copy input into a local buffer we can modify
snprintf(buf, sizeof(buf), "%s", argv[2]);

char *name = buf;

if (strncmp(name, "./", 2) == 0) {
  name += 2;
}

// 1) Strip leading "lib" if present
if (strncmp(name, "lib", 3) == 0) {
  // libfoobarr.so
  //  foobarr.so
  name += 3;
}
// 2) Strip trailing ".so" if present
size_t len = strlen(name);
if (len >= 3 && strcmp(name + len - 3, ".so") == 0) {
  // foobarr.so
  // foobarr
  name[len - 3] = '\0';
}

// 3) Build final normalized name
// expected string after trime: foobarr removed lib and .so
char result[8192];

snprintf(result, sizeof(result), "./lib%s.so", name);
