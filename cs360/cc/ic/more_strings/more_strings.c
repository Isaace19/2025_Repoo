#include <stdio.h>
#include <string.h>
typedef struct  {
	int i;
	int j;
} MyStruct;

static int sizeofarray (char *arry) {
	return sizeof(arry);
}

/*
 * to get the size of an array, you hvae to own the array or it has to be given to you, or 
 * it will just be a pointer. No other way is possible, so size has to be handed to you. 
 * */


int main (int argc, char *argv[]) {
	/*
	 * Essentially, memory is not free, and there is an O(n) lookup to find the size of a string in C
	 * In comparison to C++ where the memory lookup is O(1) since it's stored as size
	 * Same thing with copying a string should be O(n) in C
	 * */	
	MyStruct s1;
	MyStruct s2;

	// memcopy cannot overlap the memory - should do the same thing as move
	//
	// move copies backwords, which allows for overlapping memory
	// use memmove if the memory addresses overlap - destination and source
	// no problem, just use memcopy
	
	s1 = s2;
	
	int len1 = strlen(buffer); // strlen - 
	int len2 = strdup(buffer); // strdup - 
	/*
	 * Importance of O(n) lookup - every search in C will be the O(n), so we have to be precise
	 * very if this is a true statement for my notes - not so sure. 
	 * */


	char buffer[256] = "Goodbye World";

	strncpy(buffer, "Hello", 4); // strncpy -  copies up to an amount of bytes, set to 4	

	// uses the size of the buffer
	printf("%s\n", buffer);
}
