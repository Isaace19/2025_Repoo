#include "fnptr.h"
#include <stdlib.h>

union FNPTR {
    ARITHPTR afunc;
    STRPTR   sfunc;
};

FNPTR *fnptr_new_arith(ARITHPTR func){
    FNPTR *ret;
    ret = malloc(sizeof(FNPTR));
    ret->afunc = func;
    return ret;
};

FNPTR *fnptr_new_str(STRPTR func)
{
    FNPTR *ret;
    ret = malloc(sizeof(FNPTR));
    ret->sfunc = func;
    return ret;
}


double fnptr_run_double(FNPTR *fp, double left, double right) {
    return fp->afunc(left, right);
}

void fnptr_run_str(FNPTR *fp, char dst[], const char src[]) {
    fp->sfunc(dst, src);
}

void fnptr_free(FNPTR *fp) {
    free(fp);
}


