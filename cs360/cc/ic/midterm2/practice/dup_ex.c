/*
 * Basic skeleton of the usasge of the dup command
 *
 *
 *
 * */
#include <fcntl.h>
#include <stdio.h>
#include <stlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
   // dup will duplicate a file descriptor.

    char buff[1000];
    int i, fd;

    fd = open("some file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // create some child processes.
    i = fork();
    sprintf(s "fork() = %d.\n", i);
    write(fd, s, strlen(s));
    close(fd);
    return 0;
}
