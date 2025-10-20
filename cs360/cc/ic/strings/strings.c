#include <stdio.h>

int main (int argc, char *argv[]) {
	char hello[] = "Hello";
	char *world = "World";

	world[0] = 'P' // this should lead to seg fault;
	
	/*
	 * RL Data section, lower & higher half memory should be reviewed. 
	 * */


	printf("%s\n", hello);
}
