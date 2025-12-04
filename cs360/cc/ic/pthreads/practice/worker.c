#include <stddef.h>
void *worker(void *arg) {
  int *v = (int *)arg;
  *v = 1234;
  return NULL;
}
