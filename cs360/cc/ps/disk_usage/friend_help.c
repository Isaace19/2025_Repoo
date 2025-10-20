// du.h has been written for you, and it's read only.
#include "du.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// these are the prototypes given to you in du.h. You need
// to write their functionality here.

struct List *build_tree(const char starting_directory[], size_t max_depth)
{ 
  if(max_depth == 0)
  {
    return (NULL);
  }
  DIR *d = opendir(starting_directory);
  if(d == NULL)
  {
    return (NULL);
  }
  struct dirent *dent;
  struct List *head = NULL; //head of the list
  struct List *tail = NULL; //tail of the list

  while((dent = readdir(d)) != NULL)
  {
    if(strcmp(dent->d_name, ".") == 0|| strcmp(dent->d_name, "..") == 0)
    {
      continue;
    }

    struct List *temp = NULL;   
    char nextdir[1024];
    snprintf(nextdir, sizeof(nextdir), "%s/%s", starting_directory, dent->d_name);

    if(dent->d_type == DT_DIR) //recursive call if it is a directory
    {
      temp = malloc(sizeof(struct List));
      temp->name = strdup(nextdir);
      temp->type = Directory;
      temp->contents = build_tree(nextdir, max_depth -1); 
      temp->next = NULL;
    }
    else if(dent->d_type == DT_REG)
    {
      temp = malloc(sizeof(struct List));
      temp->name = strdup(nextdir);
      temp->type = File;

      struct stat st;
      stat(nextdir, &st);
      temp->size = st.st_size;
      temp->next = NULL;
    }
    if(head == NULL)
    {
      head = temp;
      tail = temp;
    }
    else
    {
      tail->next = temp;
      tail = temp;
    }
  }
  closedir(d);
  return (head);
}

void free_tree(struct List *tree) 
{
  while(tree != NULL)
  {
    struct List *next = tree->next;
    free(tree->name);
    if(tree->type == Directory)
    {
      free_tree(tree->contents);
    }
    free(tree);
    tree = next;
  }
}


size_t disk_usage(const struct List *tree)
{
  if(tree == NULL)
  {
    return (0);
  }
  size_t FinalSize = 0;
  while (tree != NULL)
  {
    if(tree->type == File)
    {
      FinalSize = FinalSize + tree->size;
    }
    else if(tree->type == Directory && tree->contents != NULL)
    {
      FinalSize = FinalSize + disk_usage(tree->contents);  
    }
    tree = tree->next;
  }
  return (FinalSize);
}

size_t file_size(const struct List *item)
{
  if(item == NULL || item->size == 0)
  {
    return (0);  
  }
  size_t FinalSize = 0;
  while (item != NULL)
  {
    if (item->type == File)
    {
      FinalSize = FinalSize + item->size;
    }
    item = item->next;
  }
  return (FinalSize); 
}



/*
// du.h has been written for you, and it's read only.
#include "du.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// these are the prototypes given to you in du.h. You need
// to write their functionality here.

struct List *build_tree(const char starting_directory[], size_t max_depth)
{ 
    // create the dirent struct as well as the head and tail for 
  struct dirent *dt;
  struct List *node = NULL; //node of the list
  struct List *tail = NULL; //tail of the list
  if(max_depth == 0){
    printf("Requires a minimum nunber of recursive depth to run.");
    return NULL;
  }
  DIR *d = opendir(starting_directory);
  if(!d)
  {
    printf("Error opening the directory\n");
    return NULL;
  }


  while((dt = readdir(d)) != NULL) {
    if(strcmp(dt->d_name, ".") == 0|| strcmp(dt->d_name, "..") == 0){
      continue;
    }

    struct List *temp = NULL;   
    char max_path[1024];
    snprintf(max_path, sizeof(max_path), "%s/%s", starting_directory, dt->d_name);

    if(dt->d_type == DT_DIR) {
      temp = malloc(sizeof(struct List));
      temp->name = strdup(max_path);
      temp->type = Directory;
      temp->contents = build_tree(max_path, max_depth -1); 
      temp->next = NULL;
    }
    else if(dt->d_type == DT_REG){
      temp = malloc(sizeof(struct List));
      temp->name = strdup(max_path);
      temp->type = File;

      struct stat st;
      stat(max_path, &st);
      temp->size = st.st_size;
      temp->next = NULL;
    }
    if(node == NULL){
      node = temp;
      tail = temp;
    }
    else
    {
      tail->next = temp;
      tail = temp;
    }
  }
  closedir(d);
  return node;
}

void free_tree(struct List *tree) 
{
  while(tree != NULL){
    struct List *next = tree->next;
    free(tree->name);
    if(tree->type == Directory){
      free_tree(tree->contents);
    }
    free(tree);
    tree = next;
  }
}


size_t disk_usage(const struct List *tree)
{
  if(tree == NULL) {
    return 0;
  }
  size_t total_size = 0;
  while (tree != NULL) {
    if(tree->type == File) {
      total_size = total_size + tree->size;
    }
    else if(tree->type == Directory && tree->contents != NULL){
      total_size = total_size + disk_usage(tree->contents);  
    }
    tree = tree->next;
  }
  return total_size;
}

size_t file_size(const struct List *item)
{
  if(item == NULL || item->size == 0){
    return 0;  
  }
  size_t total_size = 0;
  while (item != NULL){
    if (item->type == File){
      total_size = total_size + item->size;
    }
    item = item->next;
  }
  return total_size; 
}






*/