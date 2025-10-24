/* Pipe Communication
 * Write a program that does the following:
 * 1. Parent writes a list of integers into a pipe
 * 2. Child reads the numbers, then computes their sum and prints it out.
 * */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    // create pipes, and duplicate what comes out of the child side write to the parent read.
    // After the parent reads in the pipe, then we can compute that sum
    if (argc < 2) {
        fprintf(stderr, "USage: %s <inputfile>\n", argv[0]);
        return 1;
    }


    pid_t pid;
    int status;
    int mypipes[2]; // create the actual pipe
   
    /*
     * 0 = Read
     * 1 = Write
     * */

    // create the actual pipe
    pipe(mypipes);

    // start the child process to run
    pid = fork();
    if (pid == 0) {
        // if child process, then close one end of the pipe (ctp) read and take the write end to run exec
        close(mypies[1]); // close the writing end of our pipe, makes the child read

        // apply execv to get the path into the program
        char *const cat_args[] = {"cat", argv[1], NULL};

        execv("/bin/cat", cat_args);
        perror("execv");
        exit(EXIT_FAILURE);
    }

}
