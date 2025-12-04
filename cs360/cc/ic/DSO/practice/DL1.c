/*
 * This challenge set is the 10/31/2025 in class challenge code solution
 * Pracitce with the command line and DSO
 * */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
typedef void (*INPUTFUNC)(char dest[]);
typedef void (*OUTPUTFUNC)(const char src[]);

int main(int argc, char *argv[]) {
    if (3 != argc) {
        fprintf(stderr, "Usage: %s <library> <key>\n", argv[0]);
        return 1;
    }

    // working with the CLI to interface with the argv[1]
    // the big hurdle here is trying to work with prepending and appealing in an efficient way. 

    char libpath[256] = {0};
    const char *stem = argv[1];

    // file handling 
    //
    //
    // essentially we are checking if the ./, lib, and .so are on the filepath string
    // if they are, cool, but if not -> we want to prepend or append those to the libpath
    // c style string.


    if (strncmp(stem, "./", 2) != 0) {
        strcat(libpath, "./");
    }

    if (strncmp(stem, "lib", 3) != 0) {
        strcat(libpath, "lib");
    }

    strcat(libpath, stem);
    if (!strstr(libpath, ".so")) {
        strcat(libpath, ".so");
    }

    void *handle;
    handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        printf("%s\n", dlerror());
        return 1;
    }

    // can have a printf error check to know that we handle the handle allocation 

    struct Export *export = (struct Export *)dlysm(handle, "EXPORTS");
    if (!export) {
        printf("EXPORTS not found within the handle table\n");
        return 1;   
    }

    // can error check here to see if the dlsym had handled and the handle now points to 
    // a symbol

    // check the key and see if we have a match, this is just one of the libc functions

    if (strcmp(export->key, argv[2] != 0)) {
        printf("Error: key doe snot match.\n");
        return 1;
    }

    char buffer [512];
    exports->input(buffer);
    printf("Input = '%s'\n", buffer);

    // now do a reverse function:
    int length = strlen(buffer);
    int j = length - 1;
    for (int i = 0; i < j; i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
    }

    export->output(buffer);

    dlclose(handle);
    return 0;
}
