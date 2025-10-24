/*
 * Prompt: Fork and Exec
 * Write a program that performs the following:
 * 1. Forks a Child
 * 2. The child will run: "/bin/ls -l" using execl().
 * 3. The parent waits for the child and prints the exit status
 * */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        // no child process is detected
        perror("fork");
        return 1;
    }

    else if(pid == 0) {
        // in the child process
        execl("/bin/ls", "ls", "-l", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child returned the following: %d\n", WEXITSTATUS(status));
    }
    return 0;

}
