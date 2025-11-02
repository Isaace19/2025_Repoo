/*
 * This challenge set is the 10/31/2025 in class challenge code solution
 *
 * Pracitce with the command line and DSO 
 * */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
typedef void (*INPUTFUNC)(char dest[]);
typedef void (*OUTPUTFUNC)(const char src[]);

struct Export {
    INPUTFUNC input;
    OUTPUTFUNC output;
    char key[16];
};


int main(int argc, char *argv[])
{
    if (3 != argc) {
        fprintf(stderr, "Usage: %s <library> <key>\n", argv[0]);
        return 1;
    }
    
    // working with the CLI to interface with the argv[1] 
    // tne big hurdle here is trying to work with prepending and appending in an efficient way. 
    char libpath[256] = {0};
    const char *stem = argv[1];

    // file handling
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
    if(!handle) {
        printf("Error with opening the .so file: %s\n", dlerror());
        return 1;
    }
    printf("Handle allocation passed");

    // worry about the library handling real quick since the command line will be harder
    
    struct Export *export = (struct Export *)dlsym(handle, "EXPORT");
    if (!export) {
        printf("EXPORT not found within the handle table\n");
        return 1;
    }
    printf("handle table properly assigned to symbol\n");

    // check the key and see if we have a match
    if (strcmp(export->key, argv[2]) != 0) {
        fprintf(stderr, "Error: key does not match\n");
        dlclose(handle);
        return 1;
    }
    
    char buffer[512]; // for reversing input stuff
    export->input(buffer);
    printf("Input = %s\n", buffer);

    // now prepare to do the reverse
    int length = strlen(buffer);
    int j = length - 1;
    for (int i = 0; i < j; i++, j--) {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
    } 
    // call the output function of the DSO and pass it the reversed string
    export->output(buffer);
    // print the buffer 
    printf("DELETE ME\n~~~~~~~~~\nLib = %s, Key = %s\n~~~~~~~~~\n", argv[1], argv[2]);


    dlclose(handle);
    return 0;
}

