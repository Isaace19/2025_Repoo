#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fl;
    fl = fopen(argv[1], "r");
    if(!fl){
        perror(argv[1]);
        return -1;
    }

    char line[256];
    int a,b,c;
    while(fgets(line, 256, fl)) {
        if(sscanf(line, "%d %d %d", &a,&b,&c) != 3){
            break;
        }
        int result = a * b + c;
        printf("%-4d * %-4d + %-4d = %d\n", a,b,c,result);
    }

    fclose(fl);
    return 0;
}

