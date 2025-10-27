// Plank Note - Signal Handler for CNTL - C

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Driver Code for a Control C Implementation  - instead of default action ; it
// should call this handler

void cntl_c_handler(int dummy) {
  signal(SIGINT, cntl_c_handler);
  printf("You just typed cntl-c\n");
}

int main() {
  int i, j;
  // register the signal handler with the operating system
  signal(SIGINT, cntl_c_handler);

  // spin the CPU before exiting
  // todo -> learn this
  for (j = 0; j < 5000; j++) {
    for (i = 0; i < 1000000; i++) {
    }
  }
  return 0;
}