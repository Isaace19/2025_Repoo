#include <stdio.h>
#include <sys/types.h>
#include <search.h>
#include <stdlib.h>

int comp_ascending(const void *lp, const void *rp) {
    int left = *(const int *)lp;
    int right = *(const int *)rp;

    if(left < right) return -1;
    if(left > right) return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int sum = 0;
    int nums_in = 0;
    int temp;
    int max;
    int min;
    double average;
    double median;

    int line[256];

    FILE *fl = fopen(argv[1], "r");
    if (!fl) {
        perror(argv[1]);
        return (1);
    }

    while(fscanf(fl, "%d", &temp) == 1){
          // read in the file pointer and store that as the index into the temp array
        line[nums_in] = temp;
        sum = sum + temp;
        nums_in++;

        if (nums_in == 0){
            printf("Error, nothing in the array");
            return -1;
        }
    }

    // run a qsort on the lines
    // int n_size = sizeof(line) / sizeof(line[0]);
    qsort(line, nums_in, sizeof(int), comp_ascending);

    // if the array is odd, then get the middle of the array, if not do some funny things to get the median value
    if (nums_in % 2 == 0){
        median = (line[nums_in/2] + line[((nums_in - 1)/2)]) / 2.0;
    } else {
        median = line[nums_in/2];
    }

    // compute the min and max now that the array is qsort
    min = line[0];
    max = line[nums_in - 1];

    // compute the average
    average = (double)sum / nums_in;

    // print out the information with proper alignment.
	printf("Sum        = %d\n", sum);
	printf("Average    = %.2lf\n", average);
	printf("Min        = %d\n", min);
	printf("Max        = %d\n", max);
	printf("Median     = %.2lf\n", median);

    fclose(fl);
    return 0;
}

