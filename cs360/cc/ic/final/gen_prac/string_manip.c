#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
 
 // write that cuts a file at certain parts and moves it
 void cut ()
 
 
 
 
int main(int argc, char*argv[]) 
{
    char source[256];
    fgets(source, sizeof(source), stdin);
    char destination[256];
    
    char *substring;
    strcpy(destination, source);
    
    // find any substring from destination
    size_t pos;
    substring = strstr(destination, "World");
    if (substring) {
        // cut the string and move it
        pos = strcspn(destination, "W");
        destination[pos - 1] = '\0';
        strcat(destination, " - I modified this string");
    }
    
    printf("Result: %s, which replaced this: %ld\n", destination, pos);
    return 0;    
}