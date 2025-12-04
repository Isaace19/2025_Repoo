// THIS FILE IS __NOT__ GRADED.
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct Work {
  double test_val;
  int test_target;
  bool result;
};

void *dso_worker_func(void *arg) {
  struct Work *w = (struct Work *)arg;
  w->result = (w->test_target / 2000) > w->test_val;
  return NULL;
}

void dso_reporter_func(const struct Work *w) {
  if (w->result) {
    printf("Result: %7.3lf\n", w->test_val);
  }
}
