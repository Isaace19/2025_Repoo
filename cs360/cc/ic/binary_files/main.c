#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fl;
    fl = fopen(argv[1], "rb");

    if (argc < 2){
        return (1);
    }


    if (!fl){
        perror(argv[1]);
        return -1;
    }


    int temp;
    int sum = 0;
    // read integer value into file is exhausted, and add all integer values until the file is exhausted
    while (fread(&temp, 1, sizeof(temp), fl) == 4){
        sum = sum + temp;
    }
    printf("%d\n", sum);
    fclose(fl);
    return 0;
}

