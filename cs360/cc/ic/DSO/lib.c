#include <stdio.h>

int myglobal = 77;

int mylibfunc(int a) {
    printf("%d\n", a + 10);
    return a + 10;
}
