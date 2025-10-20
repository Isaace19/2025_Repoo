#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	pid_t pid = fork();

	if(pid < 0) {
		perror("Fork");
	}

	/*
	 * creates a process ID object from fork()
	 * if the process id fails, then there is a fork error.
	 *
	 * else, if the process id is 0, then we try to run execlp, which excutes the shell command
	 * ls
	 * 
	 *	This version of the program runs very similar to the execlp, however in this case we are creating an array full of the arguments.
	 *	in this case, the array of strings for args must be taken in as a execv for argument vector!
	 * 	
	 * if there's an error, then return errno, and exit on failure.
	 *
	 *
	 * waitpid is called by the parent process, waiting for the child process pid == 0 to end because parent > 0.
	 * NULL means no status, does not care about the child return flags, 0 means no special option flags
	 * */


	else if (pid == 0) {
		char *args[] = {"ls", "-a", NULL};
		execvp("ls", args);
		perror("execvp");
		exit(EXIT_FAILURE);	
	}

	waitpid(pid, NULL, 0);
}
