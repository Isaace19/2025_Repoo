// Open a file, generate a million random numbers, then store inside 1mill.c
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// thank jim macnamara for this code. 
#define ARRSIZE 10000000

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int r = rand();

    FILE *fd = fdopen("1bill.txt", O_RDWR);
    if (!fd) {
        perror("fdopen");
        return 1;
    }
    


    int i;
    for (i = 0; i < 1000000000; i+=1) {
        // generate 1 billion random numbers
         
    }
    

    close(FILE);
}
