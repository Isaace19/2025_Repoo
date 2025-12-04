#include <stdio.h>

// This file IS NOT graded, but it can be
// used to test your server.

// When using the "Run" button above, it will
// be passed the name of a socket to use with the
// server.

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <socket>\n", argv[0]);
        return 1;
    }
    printf("TESTER: connect to socket '%s'\n", argv[1]);
    return 0;
}

