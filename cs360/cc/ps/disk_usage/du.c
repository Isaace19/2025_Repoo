// du.h has been written for you, and it's read only.
#include "du.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

// these are the prototypes given to you in du.h. You need
// to write their functionality here.

typedef struct List List;


struct List *build_tree(const char starting_directory[], size_t max_depth) {
   /* depth==0 should not error; just stop descending later */
   // if(max_depth == 0) {
   //      printf("Error, requires recursive depth of at least 1\n");
   //      return NULL;
   // }
   // set the stat structure
   struct stat st;
   /* lstat should be called; if it fails, return NULL */
   if (lstat(starting_directory, &st) == 0) {
        perror("lstat");
        return NULL;
   }
   
   // allocate memory for the singly linked list node
    List *node = malloc(sizeof(List));
    if (node == NULL) {
        printf("Bad memory allocation for node object\n");
        return NULL;
    }
   node->next = NULL; // set up next pointer to NULL
   node->name = strdup(starting_directory); // copy string to the name pointer
   if(node->name == NULL){
        printf("Name not allocated\n");
        free(node);
        return NULL;
   }

   // check the input directory and files given the stat struct macros
   if (S_ISREG(st.st_mode)) {
        node->type = File;
        node->size = st.st_blocks * 512;
        return node;  /* return immediately for files */
   } else if (S_ISDIR(st.st_mode)) {
        node->type = Directory;
        node->contents = NULL;

        if (max_depth <= 1) {
            // can't do recursion on this step size, therefore we should output the current node size.    
            return node; 
        }
        // open the directory
        DIR *d;
        d = opendir(starting_directory);
        if (!d){
            printf("Could not open the directory\n");
            free(node->name);
            free(node);               /* avoid leak on failure */
            return NULL;
        }

            struct dirent *next;
            struct List *prev = NULL;
            while((next = readdir(d)) != NULL) {
                if (strcmp(next->d_name, ".") == 0 || strcmp(next->d_name, "..") == 0) {
                    continue;
                }
                char max_len[PATH_MAX];
                int n = snprintf(max_len, PATH_MAX, "%s/%s", starting_directory, next->d_name);
                // after storing this path, recursively build the tree on this path
                if (n < 0 || n>=(int) sizeof(max_len)){
                    continue;
                }
                struct List *child_node = build_tree(max_len, max_depth - 1);
               if (child_node == NULL) {
                    continue; // error with child node initialization
               }
               if (!node->contents) {
                    node->contents = child_node;
               }else {
                    prev->next = child_node;
               }
                prev = child_node;

            }
    closedir(d);
    } else {
        /* unsupported types (symlink, device, fifo, socket): skip */
        free(node->name);
        free(node);
        return NULL;
    }
    return node;  
}
   
void free_tree(struct List *tree){
    if (tree == NULL) {
        return;
    }
    if (tree->type == Directory) {
        struct List *current = tree->contents;
        while (current != NULL) {
            struct List *next = current->next;
            free_tree(current);
            current = next;
        }
    }
    free(tree->name);
    free(tree);
    return;
}

size_t disk_usage(const struct List *tree){
    if (!tree){
        return 0;
    }

    if (tree->type == File) {
        return tree->size;
    } 
    size_t total = 0;
    for (const struct List *ch = tree->contents; ch; ch=ch->next) {
        total += disk_usage(ch);
    }
    return total;
}


size_t file_size(const struct List *item) {
    if (!item) {
        return 0;
    }

    if (item->type == File) {
        return item->size;
    }    

    size_t total = 0;
    for(const struct List *ch = item->contents; ch; ch = ch->next)
        if (ch->type == File) {
            total += ch->size;
        }
    return total;
}
