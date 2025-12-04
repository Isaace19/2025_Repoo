#include "plugin.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Write your substitute plugin here

char upper[27];
char lower[27];

void init(void) {
  // init default upper and lower to be the alphabet
  strcpy(upper, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  strcpy(lower, "abcdefghijklmnopqrstuvwxyz");
}

PluginResult setting(const char name[], const char value[]) {
  size_t converted_value = value ? strlen(value) : 0;
  if (converted_value > 26) {
    return PR_FAILED;
  }
  if (strcmp(name, "upper") == 0 && converted_value <= 26) {
    for (int i = 0; i < converted_value; i++) {
      upper[i] = value[i];
    }
    return PR_SUCCESS;
  }
  if (strcmp(name, "lower") == 0 && converted_value <= 26) {
    for (int i = 0; i < converted_value; i++) {
      lower[i] = value[i];
    }
    return PR_SUCCESS;
  }
  return PR_FAILED;
}

PluginResult transform(char *data, int *size) {
  for (int i = 0; i < *size; i++) {
    char temp = data[i];
    if (isupper(temp)) {
      int temp1 = temp - 'A';
      if (temp1 >= 0 && temp1 < 26) {
        data[i] = upper[temp1];
      }
    } else if (islower(temp)) {
      int temp1 = temp - 'a';
      if (temp1 >= 0 && temp1 < 26) {
        data[i] = lower[temp1];
      }
    }
  }
  return PR_SUCCESS;
}
