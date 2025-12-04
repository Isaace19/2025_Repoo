/*
11/03/2025
Code from in class sockets lecture

Server - wait connection
Client - makes connection

Streaming Socket
Datagram - set of bytes

Sockets - file descriptors

Sock_stream
Sock_DGRAM

Address Family: Internet & Unix
AF_INET
AF_UNIX
AF_INET6

*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
// sockfd = socket(int socket_family, int socket_type, int protocol);

// with sockets, we have to deal with big endian tosend/receive data for
// networks. network byte order must be in big endian when sending information
// over the internet

// DNS - domain name server

int main(int argc, char *argv[]) {
  // unsigned int data = 0xdeadbeef;
  // fwrite(&data, 1, sizeof(data), stdout);

  int s;
  // always takes 3 inputs for socket 
    // always 0 in this class since we won't have root access
    //
    // returns -1 on error, or a descriptor of the socket
  s = socket(AF_UNIX, SOCK_DGRAM, 0);
   if (-1 == s) {
        perror("socket");
        return 1;
    }
    // client side - create socket connect socket
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    
    struct sockaddr_in sin = {0};
    
    // fill in fields
    // polymorphism/polymorphic - this is the void * 
    // means that we can do inheritence 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    unsigned int a = 0;
    // shift by bits
    a |= (64 << 24);
    a |= (233 << 16);
    a |= (176 << 8);
    a |= (100);
    sin.sin_addr.s_addr = htonl(a);
    if (-1 == connect(s, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        return 1;
    }
    // client - create socket first, then connect it
    

}

// socket address in UNIX sockaddr_un

struct sockarr_un {
  sa_family_t sun_family;
  char sun_path[];
};
