#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    // process text from the file, in the WORDTEXT format
    char* dir;
    int length;
}Data;


// basics of reading in files: want to read in a file
// then prcocess every line and do something with it.


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error with reading in the input file.\n");
        return 1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        perror("fopen");
        return 1;
    }


    return 0;
}
