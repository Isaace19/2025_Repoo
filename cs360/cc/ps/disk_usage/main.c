#include <stdio.h>
#include <stdlib.h>
#include "du.h"

extern void free_tree(struct List *root);


int main(int argc, char *argv[])
{
    // Write anything you want to test your du.c/du.h files.
    // This file will NOT be tested. Only du.c/du.h will be.
    printf("Hello, World!\n");

    if (argc < 2) {
        printf("Error, not enough arguments\n");
        return 1;
    }

    struct List *root = build_tree(argv[1], 5);
    if (!root) {
        printf("Build tree function failed\n");
        return 1;    
    }

    printf("Built tree for: %s\n", root->name);
    free_tree(root);
    return 0;
}
