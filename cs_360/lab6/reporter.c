#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int main() 
{
    uint64_t value;
    uint64_t size;
    uint64_t i;

    read(STDIN_FILENO, &size, sizeof(size));
    for (i = 0;i < size;i+=1) {
        read(STDIN_FILENO, &value, sizeof(value));
        printf("%lu\n", value);
    }

    return 0;
}


