/*
 * dlfcn.h functions will be expected on the midterm 3 - so study all 4 functions in this header and how to implement them. 
 *
 * */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

// write something that will handle a global .so file that gets linked to this .c file.

typedef int( * myfunc) (int a, int b);

int main() {
    myfunc func;

    void *handle;
    int i, j;

    handle = dlopen("./plib.so", RTLD_LAZY | RTLD_GLOBAL);
    if (handle == NULL) {
        printf("Error with plib.so: %s\n", dlerror());
        return 1;
    }
    printf("Handle allocation passed\n");
   
    func = (myfunc)dlsym(handle, "add");
    if (NULL == func) {
        printf("const *char name not found within handle table\n");
        return 1;
    }
    printf("Name found within handle!\n");

    i = func(15, 10); 
    printf("func result is: %d\n", i);

    printf("Func returned %d\n", i);
    printf("Func returned %d\n", j);
    dlclose(handle);

    return 0;
}
