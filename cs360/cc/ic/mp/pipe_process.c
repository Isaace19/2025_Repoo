#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {

int parent_to_child[2];
int child_to_parent[2];

pipe(parent_to_child);
pipe(child_to_parent);

if (fork() != 0) {
	//parent
	close(parent_to_child[0]); // parent to child read
	close(child_to_parent[1]); // child to parent write
}

else {
	// child
	close(parent_to_child[1]); // parent to child write
	close(parent_to_child[0]); // child to parent read

	dup2(parent_to_child[0], STDIN_FILENO); // child stdin to parent write pipe
	dup2(child_to_parent[1], STDOUT_FILENO); // child stdout to the parent read pipe
}

	
}
