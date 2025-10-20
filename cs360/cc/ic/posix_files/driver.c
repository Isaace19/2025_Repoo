#include <stdio.h>
#include <string.h>

int open_file(const char *file_name);
int read_bytes(int fd, char bytes[], int max_size);
void seek(int fd, int offset, int whence);
int tell(int fd);
void close_file(int fd);

int main()
{
    int score = 0;
    int fd;
    int num;
    char buffer[256];

    fd = open_file("notreal.txt");
    if (fd == -1) {
        score += 1;
    }
    else {
        printf("FAIL: opened an invalid file, but didn't get -1\n");
    }

    fd = open_file("test.txt");
    if (fd <= 0) {
        printf("FAIL: opened valid file, but it didn't work.\n");
    }

    num = read_bytes(fd, buffer, 11);
    buffer[11] = '\0';
    if (strcmp(buffer, "Hello World")) {
        printf("FAIL: read bytes from file, but didn't get the right ones.\n");
    }
    else if (num != 11) {
        printf("FAIL: tried to read bytes, but didn't read them all.\n");
    }
    else {
        score += 1;
    }
    num = read_bytes(fd, buffer, 1);
    buffer[1] = '\0';
    if (strcmp(buffer, "!")) {
        printf("FAIL: read more bytes, but didn't get it right.\n");
    }

    num = tell(fd);
    if (num != 12) {
        printf("FAIL: asked tell, and didn't get the right offset.\n");
    }
    else {
        score += 1;
    }

    seek(fd, -1, SEEK_END);
    num = read_bytes(fd, buffer, 1);
    buffer[1] = '\0';
    if (strcmp(buffer, "\n")) {
        printf("FAIL: seeked to a spot, but didn't get the right data.\n");
    }
    else {
        score += 1;
    }
    num = tell(fd);
    if (num != 25) {
        printf("FAIL: seeked and didn't get right spot.\n");
    }
    else {
        score += 1;
    }

    close_file(fd);


    return score;
}

