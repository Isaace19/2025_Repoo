#include <string.h>
#include <stdio.h>
#include "plugin.h"

// Skip plugin demonstration

char skip_char;

// If defined, init will be called when the plugin is loaded
// after a -f switch.
void init(void)
{
    // By default skip spaces
    skip_char = ' ';
}

// The setting function is called each time a setting is given to
// a plugin after -f with the switch -s name=value (setting).
// The name part goes into the parameter name and the value part goes
// into the value parameter.
PluginResult setting(const char name[], const char value[])
{
    if (!strcmp(name, "skip") || !strcmp(name, "char")) {
        // If the name portion of the setting is called skip OR char
        // then we reset the skip character.
        if (strlen(value) != 1) {
            // Make sure we're given only one character
            printf("Value given for skip setting is not a single character.\n");
            // If we return PR_FAILED, then the plugin loader will print an error
            // message, but if we return PR_STOP, it won't.
            return PR_STOP;
        }
        // Set the skip_char to the character. We ensured above that the string
        // in value is exactly one character, not including the \0.
        skip_char = value[0];
        return PR_SUCCESS;
    }
    // If we get here, then a name not skip or char was given. Since we don't
    // print an error message, the error message will be printed by the plugin
    // loader.
    return PR_FAILED;
}

// The transform function is called after the plugin is loaded and after all
// settings have been applied. We are given a memory buffer and the size of
// that memory buffer. We transform it in place, and whatever is in the buffer
// when this function returns is what the plugin loader "gets".
PluginResult transform(char *data, int *size)
{
    int copy_to;
    int copy_from;
    char c;

    for (copy_to = copy_from = 0;copy_from < *size;copy_from += 1) {
        c = data[copy_from];
        if (c != skip_char) {
            data[copy_to] = c;
            copy_to += 1;
        }
    }
    *size = copy_to;
    return PR_SUCCESS;
}

