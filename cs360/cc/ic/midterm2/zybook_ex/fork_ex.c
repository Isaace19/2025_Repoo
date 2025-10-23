#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t pid;
  if (argc < 2) {
    printf("Usage: %s: <command> [args]\n", argv[0]);
    return 1;
  }

  pid = fork();
  if (pid < 0) {
    perror("Fork");
  }

  if (pid == 0) {
    execv(argv[1], argv + 1);
    perror("execv");
    return 1;
  }
  int status;
  waitpid(pid, &status, 0);
  if (WIFEXITED(status)) {
    fprintf(stderr, "Child returned the following: ", WEXITSTATUS(status));
  }
  return 0;
}
