#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

// generic structure for IPV4:

// struct sockaddr_in {
//     sa_family_t sin_family; // AF_INET
//     in_port_t sin_port;     // Port num
//     struct in_addr sin_addr; // IPV4 Addr
// };
//
// struct in_addr {
//      uint32_t s_addr;
// }

int main(int argc, char *argv[]) {
    int i;
    //for (i = 1; i < argc; i += 1) {
    //    printf("argv[%d] = %s\n", i, argv[i]);
    //}

      /* Constructing and sending the HTTP GET request below
       * GET /file/path HTTP/1.1
       * Host: host.name.com
       * Connection: close
       * */

    if (argc != 4) {fprintf(stderr, "Not Enough Arguments\n"); return 1;}
    char* host = argv[1];
    char* port = argv[2];
    char* fp   = argv[3];
    int sfd, s;
    char request[256];
    // formats the message to send to the server.
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", fp, host);

    //create the necessary structures for the IPV4 socket creation and connection
    struct addrinfo hints = {0};
    struct addrinfo *result, *rp;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    // from the linux man pages
    s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s)); return 1;}

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (-1 == sfd) {
            continue;
        }
        else if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sfd);
    }
    freeaddrinfo(result);
    if (rp == NULL) {
        fprintf(stderr, "Could not bind the socket\n");
        return 1;
    }
    // read in the data from the socket
    ssize_t bytes = 0;
    bytes = send(sfd, request, strlen(request), MSG_NOSIGNAL);
    if (-1 == bytes) {perror("send"); close(sfd); return 1;}


    // trim the header and body from the server message:
    char head_buff[8192] = {0};
    // track the length and endline of the header part of the message:
    size_t head_len = 0;
    int header_done = 0;

    char buffer[256] = {0};
    while (0 < (bytes = recv(sfd, buffer, sizeof(buffer) - 1, 0))) {
        buffer[bytes] = '\0';
        if (!header_done) {
            // if not the end of line of the header, then we want to copy
            // the message:
            if (sizeof(head_buff) - 1 > head_len + bytes) {
                memcpy(head_buff + head_len, buffer, bytes + 1);
                head_len = head_len + bytes;
            }
            char *term = strstr(head_buff, "\r\n\r\n");
            if (term) {
                header_done = 1;
                term = term + 4;
                printf("%s", term);
            }
        }else {
            printf("%s", buffer);
        }
    }

    close(sfd);
    return 0;
}

