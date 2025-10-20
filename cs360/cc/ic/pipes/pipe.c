#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
	pid_t pid;
	int status;
	int mypipes[2];
	
	pipe(mypipes);
	pid = fork();
	if(pid == 0){
		close(mypipes[0]);
		execlp("ls", "ls", "-1", NULL);
		perror("execlp");
		return 1;
	}
	
	// experimenting with child & parent
	// wanting to print out the entire directory data, just seems to only print out working dir.	
	


	close(mypipes[1]);

	char data[256];
	// create a while loop
	
	int bytes;
	while(0 < (bytes = read(mypipes[0], data, 255))) {
		data[bytes] = '\0';
		printf("%s", data);
	}


	//bytes = read(mypipes[0], data, 255);
	//data[bytes] = '\0';

	close(mypipes[1]);
	waitpid(pid, &status, 0);

	printf("%s", data);
	//printf("%ld", strlen(data));
	return 0;
}
