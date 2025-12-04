/*
 *
 * THIS IS FROM THE INCLASS ZYBOOK LAB ON SERVER SIDE PROGRAMMING 
 * IN SOCKETS

 *
 * */

#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>

static char *update(char data[]);

int main(int argc, char *argv[])
{
    if (2 != argc) {
        fprintf(stderr, "Usage: %s <socket>\n", argv[0]);
        return 1;
    }

    struct sockaddr_un sun = {0};
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    int srvsock;
    int clnsock;
    char data[128];
    ssize_t bytes;

    srvsock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (-1 == srvsock) {
        perror ("Socket"); 
        return 1;
    }

    // needs the sun family
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, argv[1], sizeof(sun.sun_path) - 1); 
    // array of that many bytes, sizeof does not include null terminate, has to be initialized to 0
    // to get rid of all null terminates for this. 
    // binds to the system, next we listen
    if (-1 == bind(srvsock, (struct sockaddr*)&sun, sizeof(sun))) {
        perror ("bind");
        return 1;
    }

    listen(srvsock, 5); // means it can handle states

    // with server socket, we just accept connections from it, so we accept srvsock and socket address, size
    client_addr_len = sizeof(client_addr);
    clnsock = accept(srvsock, (struct sockaddr*)&client_addr, &client_addr_len);
    if (-1 == clnsock) {
        perror("accept");
        // to remove file from system, we have to decrement the pointer through unlinking the argv
        // however this has to be done with a goto statement
        // acts like a deconstructor

        // "unwinding" undoing all the actions, un listen, unbind, close socket - do the reverse of what we did
        goto out;
    }
    // get data from client with receive
    bytes = recv(clnsock, data, sizeof(data) - 1, MSG_NOSIGNAL);
    // returns the number of bytes, a signed size_t since it gives -1 on error. 
    if (-1 == bytes) {
        perror("recv");
        goto out;
    }
    // we received data from data buffer, however it is not null terminated
    data[bytes] = '\0';
    // now we have a c-style string
    // now we can use our functions on it since it has a null terminator

    bytes = send(clnsock, update(data), strlen(data), MSG_NOSIGNAL);
    if (-1 == bytes) {
        perror("send");
        goto out;
    }    
out:

    close(srvsock);
    unlink(argv[1]);
    return 0;
}

static char *update(char data[])
{
    const int len = strlen(data);
    int i;
    // look through each char

    for (i = 0; i < len; i+=1) {
        if (data[i] >= 'e') {
            data[i] = toupper(data[i]);
        }
    }
    
    return data;
}

