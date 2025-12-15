#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>include <stdio.h>
#include "pagealloc.h"

// Helper function for testing: print the first bookkeeping byte
extern unsigned char *bookkeep; // Only for debugging, not for production

int main(int argc, char* argv[]) {
    page_init(8);
    void *p = page_alloc(4);
    if (!p) {
        printf("alloc failed\n");
        return 1;
    }
    // Print the first bookkeeping byte (should be 0xab for 4-page alloc)
    printf("bookkeep[0] = 0x%02x\n", bookkeep[0]);
    page_deinit();
    return 0;
}
