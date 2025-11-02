/*
Date: 10/27/2025

.so - shared object
.a - static archive

creating a DSO - PLUGIN Lab
dynamic shared objects
vdso - virtual dynamic shared object

-static compile command
Marz is an Arch Linux user btw.

elf - executable linkable format

Header: dlfcn.h
Library: -ldl
dynamic linker for dl open or dl close


MIDTERM 3 - PTHREADS & DSO


*/

#include <dlfcn.h>
#include <stdio.h>

typedef int (*myfunc)(int a);

int main() {
  // printf("Hello World\n");
  myfunc func;
  void *handle;
  int i;
  int *j;

  /* look in the current working directory
   * Lazy relocation Loading
   * relocation is performed when needing, low initial overhead, but high cost
   * to calling
   * */

  handle = dlopen("./libmy.so", RTLD_LAZY | RTLD_GLOBAL);

  if (NULL == 0) {
    printf("Error with libmy.so: %s\n", dlerror());
    return 1;
  }

  func = (myfunc)dlsym(handle, "mylibfunc");

  if (NULL == func) {
    printf("Error with mylibfunc; %s\n", dlerror());
    return 1;
  }
  i = func(77);
  printf("Func returned %d\n", i);

  // dlysm gives you the memory address of where it puts the value
  j = (int)dlsym(handle, "myglobal");
  printf("My global = %d\n", *j);

  dlclose(handle);
  /* expects a seg fault because?
   * what happened is that dlclose just freed *handle, so it no longer exists
   * and causes the seg fault
   *
   * would dereferencing save this from seg fault?
   * c has no exception bounds checking, we only have codes
   * EOF - set to negative 1 (-1)
   * */
  printf("Func returned %d\n", *j);
}
