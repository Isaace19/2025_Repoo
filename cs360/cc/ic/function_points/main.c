#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "fnptr.h"

static double myarith1(double left, double right);
static double myarith2(double left, double right);
static void   mystr1(char dst[], const char src[]);
static void   mystr2(char dst[], const char src[]);

int main()
{
    FNPTR *a1;
    FNPTR *a2;
    FNPTR *s1;
    FNPTR *s2;
    double res;
    char dest[65];

    a1 = fnptr_new_arith(myarith1);
    a2 = fnptr_new_arith(myarith2);
    s1 = fnptr_new_str(mystr1);
    s2 = fnptr_new_str(mystr2);

    res = fnptr_run_double(a1, -7.5, 2.3);
    printf("fnptr_run_double(a1, -7.5, 2.3) = %lf\n", res);
    res = fnptr_run_double(a2, 100.0, 0.7);
    printf("fnptr_run_double(a2, 100.0, 0.7) = %lf\n", res);

    fnptr_run_str(s1, dest, "Hello World");
    printf("fnptr_run_str(s1, dest, \"Hello World\") = %s\n", dest);
    fnptr_run_str(s2, dest, "Goodbye World");
    printf("fnptr_run_str(s1, dest, \"Goodbye World\") = %s\n", dest);

    fnptr_free(a1);
    fnptr_free(a2);
    fnptr_free(s1);
    fnptr_free(s2);
    return 0;
}

static double myarith1(double left, double right)
{
    return left + right;
}
static double myarith2(double left, double right)
{
    return left * right;
}
static void   mystr1(char dst[], const char src[])
{
    int i;
    int j;
    for (j = 0, i = strlen(src) - 1;i >= 0;i -= 1, j += 1) {
        dst[j] = src[i];
    }
}
static void mystr2(char dst[], const char src[])
{
    int i;
    int len = strlen(src);
    char c;
    for (i = 0;i < len;i+=1) {
        c = src[i];
        if (isupper(c)) {
            c = tolower(c);
        }
        else if (islower(c)) {
            c = toupper(c);
        }
        dst[i] = c;
    }
}


