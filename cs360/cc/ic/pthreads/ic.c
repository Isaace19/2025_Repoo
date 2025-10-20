#include <stdio.h>
#include <fcntl.h>

// persistent memory allocated as long as the program is running, regardless of scope.
vector myvector(100);

int main() {
	// allows pointer airthmetic
	int *i;

	// dynamic allocated version with stack var
	int *i = malloc(sizeof(int));
	/*
	 * why is C++ style array size allocation bad in C?
	 *
	 * no optimizations peformed by the compiler
	 *
	 * mem in stack ~= 8 mbs roughly on Hydra, 4k on windows 10
	 * 
	 * 
	 *
	 * */

	int size = 100;
	int j[size];

	vector myvector(100); // created - constructed - at compile time at line 24
	// not referenced counted, only breaks after main exit
	
	// persistent memory in C++:
	/*
	 * make it heap allocated mem. 
	 * requires the delete keyword in order to free memory, will not destroy after exit scope
	 *
	 * */



	printf("%d\n", i);
	return 0;
}
