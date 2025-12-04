#include "plugin.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Write uppercase code here

// make a single toupper char function that turns one char to uppercase

// int toupper(int ch);
int stride_length;
char ch;

void init(void) {
  // by default string is  = 1
  stride_length = 1;
}

PluginResult setting(const char name[], const char value[]) {
  // I should be able to skip a certain amount (stride) per the length of char
  // name so I think that value will replace stride if value is given anything.
  int converted_value = atoi(value);

  // look for keyword stride as name and than char value is stride length fml
  if (!strcmp(name, "stride") || converted_value > 0) {
    stride_length = converted_value;
    return PR_SUCCESS;
  }
  return PR_FAILED;
}
PluginResult transform(char *data, int *size) {
  // in transform is where we actually convert the string to uppercase
  int i;

  for (i = 0; i < *size; i += stride_length) {
    data[i] = toupper((unsigned char)data[i]);
  }
  return PR_SUCCESS;
}