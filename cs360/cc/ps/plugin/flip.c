#include "plugin.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Write your flip plugin here

// also no init or settings here

PluginResult transform(char *data, int *size) {
  char temp[*size];
  size_t line_start[1024]; // just something arbitrary that should resize
  size_t line_count = 0;
  int i;

  line_start[line_count++] = 0; // first line will always start with 0
  for (i = 0; i < *size - 1; i++) {
    // go through the entire buffer, and indicate every newline detected
    if (data[i] == '\n' && i + 1 < *size) {
      line_start[line_count++] = i + 1;
    }
  }
  // for every newline, remember where they begin.
  size_t temp_index = 0;
  for (size_t line = line_count; line-- > 0;) {

    size_t start = line_start[line];
    size_t end = (line + 1 < line_count) ? line_start[line + 1] - 1 : *size - 1;

    size_t length = end - start + 1;

    if (line == 0 && data[*size - 1] != '\n') {
      length = end - start;
    }
    memcpy(temp + temp_index, data + start, length);
    temp_index += length;
  }
  memcpy(data, temp, *size);
  return PR_SUCCESS;
}
