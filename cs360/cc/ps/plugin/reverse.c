#include "plugin.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Write your reverse plugin here

// don't have to worry about init and setting for this, just need to reverse
// in transform

PluginResult transform(char *data, int *size) {
  size_t left = 0;
  size_t right = *size - 1;
  while (left < right) {
    char temp = data[left];
    data[left] = data[right];
    data[right] = temp;
    left++;
    right--;
  }
  return PR_SUCCESS;
}