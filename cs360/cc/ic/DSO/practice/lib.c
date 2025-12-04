#include <stdio.h>
#include <string.h>

void inputter(char dest[])
{
        strcpy(dest, "This is input!");
}

void outputter(const char src[])
{
        printf("The stuff I got was '%s'\n", src);
}


typedef void (*INPUTFUNC)(char dest[]);
typedef void (*OUTPUTFUNC)(const char src[]);

struct {
    INPUTFUNC input;
    OUTPUTFUNC output;
    char key[16];
} EXPORTS = {
    inputter,
    outputter,
    "hello_world"
};



