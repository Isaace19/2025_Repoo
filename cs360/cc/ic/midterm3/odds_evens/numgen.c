#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    const char*filename = "numbers.txt";
    const int N = 1000000;

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    srand((unsigned) time(NULL));
    int i;
    for (i = 0; i < N; i++) {
        // from a range of 0 to RAND_MAX
        fprintf(fp, "%d\n", rand());
    }

    fclose(fp);
    return 0;
}
