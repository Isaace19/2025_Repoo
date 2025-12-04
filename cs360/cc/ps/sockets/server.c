#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define BACKLOG 5
#define HELLO "HELLO"
#define HLEN 5
#define MAXC FD_SETSIZE // for select() is a macro

// This file WILL be graded.

// The Run button will compile both server.c and test.c.
// It will then run this server, and run the tester
// to connect to it.
// The server will be placed into the background and sent
// the SIGINT signal after 2 seconds.
// Your program will be killed after 5 seconds.

static volatile sig_atomic_t stop = 0;
void handle_sigint(int s) {
  (void)s;
  stop = 1;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <socket file>\n", argv[0]);
    return 1;
  }
  // printf("SERVER: bind to socket file '%s'\n", argv[1]);
  // return 0;

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("Socket");
    return 1;
  }

  // define the structure to create server side sockets.
  struct sockaddr_un addr = {0};
  addr.sun_family = AF_UNIX;

  if (strlen(argv[1]) >= sizeof(addr.sun_path)) {
    fprintf(stderr, "socket path not compatible\n");
    close(sfd);
    return 1;
  }
  strncpy(addr.sun_path, argv[1], sizeof(addr.sun_path) - 1);
  // bind and listen
  bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
  listen(sfd, BACKLOG);

  // handle the struct for timeval to begin time multiplexing
  fd_set fds, temp;
  FD_ZERO(&fds);
  FD_SET(sfd, &fds);
  int tfd = sfd;

  char data[256];
  ssize_t bytes;

  for (;;) {
    if (stop)
      break;

    temp = fds;
    struct timeval tv = (struct timeval){0, 0}; // poll
    int act = select(tfd + 1, &temp, NULL, NULL, &tv);
    if (act < 0) {
      perror("select");
      continue;
    }
    if (act == 0) {
      struct timespec ts = {0, 100000}; // 100,000 ns
      nanosleep(&ts, NULL);
      continue;
    }

    // New client?
    if (FD_ISSET(sfd, &temp)) {
      int ncc = accept(sfd, NULL, NULL);
      if (ncc >= 0) {
        // Server sends HELLO (exactly 5 bytes)
        send(ncc, HELLO, HLEN, MSG_NOSIGNAL);

        // Wait up to 1s for client HELLO
        fd_set stagger;
        FD_ZERO(&stagger);
        FD_SET(ncc, &stagger);
        struct timeval tw = {1, 0};
        int rv = select(ncc + 1, &stagger, NULL, NULL, &tw);
        if (rv > 0) {
          char head_buf[HLEN];
          bytes = recv(ncc, head_buf, HLEN, MSG_NOSIGNAL);
          if (bytes == HLEN && memcmp(head_buf, HELLO, HLEN) == 0) {
            FD_SET(ncc, &fds);
            if (ncc > tfd)
              tfd = ncc;
          } else {
            close(ncc);
          }
        } else {
          close(ncc);
        }
      }
      // do not 'continue' here; other clients may also be ready this tick
    }

    // Handle readable clients
    for (int i = 0; i <= tfd; i++) {
      if (i == sfd)
        continue;
      if (!FD_ISSET(i, &temp))
        continue;

      bytes = recv(i, data, sizeof(data) - 1, MSG_NOSIGNAL);
      if (bytes <= 0) {
        close(i);
        FD_CLR(i, &fds);
        continue;
      }
      data[bytes] = '\0';

      // BYE
      if (strncmp(data, "BYE", 3) == 0) {
        close(i);
        FD_CLR(i, &fds);
        continue;
      }

      // A\t<name> or AAAA\t<name>
      if (strncmp(data, "A\t", 2) == 0 || strncmp(data, "AAAA\t", 5) == 0) {
        char *tab = strchr(data, '\t');
        if (!tab)
          continue;
        char *host = tab + 1;

        // Trim CR/LF if present in this chunk
        char *end = host + strlen(host);
        while (end > host && (end[-1] == '\r' || end[-1] == '\n'))
          *--end = '\0';

        struct addrinfo hints, *res = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = (data[1] == '\t') ? AF_INET : AF_INET6;

        int rc = getaddrinfo(host, NULL, &hints, &res);
        if (rc != 0 || !res) {
          send(i, "NOTFOUND", 8, MSG_NOSIGNAL);
          if (res)
            freeaddrinfo(res);
          continue;
        }

        char string[128];
        memset(string, 0, sizeof(string));
        if (res->ai_family == AF_INET) {
          struct sockaddr_in *s4 = (struct sockaddr_in *)res->ai_addr;
          unsigned char *ipv4 = (unsigned char *)&s4->sin_addr;
          sprintf(string, "%03u.%03u.%03u.%03u", ipv4[0], ipv4[1], ipv4[2],
                  ipv4[3]);
        } else if (res->ai_family == AF_INET6) {
          struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)res->ai_addr;
          unsigned short *ipv6 = (unsigned short *)&s6->sin6_addr;
          for (int j = 0; j < 8; j++) {
            unsigned short seg = ntohs(ipv6[j]);
            sprintf(string + strlen(string), "%04x", seg);
            if (j < 7) {
              strcat(string, ":");
            }
          }
        }

        if (string[0] == '\0') {
          send(i, "NOTFOUND", 8, MSG_NOSIGNAL);
        } else {
          char ok[256];
          int length = snprintf(ok, sizeof(ok), "OK\t%s", string);
          if (length > 0)
            send(i, ok, (size_t)length, MSG_NOSIGNAL);
        }
        freeaddrinfo(res);
      }
    }
  }

  close(sfd);
  unlink(argv[1]);
  return 0;
}
