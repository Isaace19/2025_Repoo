#include "hfactor.h"
#include <stdio.h>

// WARNING: Grading is slow due to large numbers being tested!
// Test manually first before grading!

// The RUN button in Zylabs will compile as:
// gcc -o /tmp/Reporter reporter.c
// gcc -o /tmp/Hfactor hfactor.c main.c -lrt

// It will then run as follows:
// /tmp/Hfactor 1778 4 | sort -n
// 2
// 7
// 14
// 127
// 254
// 889

int main(int argc, char *argv[]) {
    uint64_t number_to_test;
    uint64_t workers;

    if (argc < 3) {
        printf("Usage: %s <number to test> <workers>\n", argv[0]);
        return -1;
    }
    if (1 != sscanf(argv[1], "%lu", &number_to_test)) {
        printf("Unable to scan number.\n");
        return -1;
    }
    if (1 != sscanf(argv[2], "%lu", &workers)) {
        printf("Unable to scan workers.\n");
        return -1;
    }
    printf("TESTER RUNNING /tmp/Reporter\n");
    return hfactor_calculate("/tmp/Reporter", number_to_test, workers);
}


