#pragma once

union FNPTR;
typedef union FNPTR FNPTR;

typedef double (*ARITHPTR)(double left, double right);
typedef void (*STRPTR)(char dst[], const char src[]);

FNPTR *fnptr_new_arith(ARITHPTR func);
FNPTR *fnptr_new_str(STRPTR func);
double fnptr_run_double(FNPTR *fp, double left, double right);
void   fnptr_run_str(FNPTR *fp, char dst[], const char src[]);
void   fnptr_free(FNPTR *fp);


