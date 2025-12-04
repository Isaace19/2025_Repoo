#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
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
#define MAXC FD_SETSIZE

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

static volatile sig_atomic_t stop = 0;
void handle_sigint(int s) {
  (void)s;
  stop = 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <socket file>\n", argv[0]);
    return 1;
  }
  signal(SIGINT, handle_sigint);
  unlink(argv[1]);

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd < 0) {
    perror("socket");
    return 1;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (strlen(argv[1]) >= sizeof(addr.sun_path)) {
    fprintf(stderr, "socket path too long\n");
    close(sfd);
    return 1;
  }
  strncpy(addr.sun_path, argv[1], sizeof(addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(sfd);
    return 1;
  }
  if (listen(sfd, BACKLOG) < 0) {
    perror("listen");
    close(sfd);
    unlink(argv[1]);
    return 1;
  }

  fd_set fds, temp;
  FD_ZERO(&fds);
  FD_SET(sfd, &fds);
  int tfd = sfd;

  char data[1024];
  ssize_t bytes;

  for (;;) {
    if (stop)
      break;

    temp = fds;
    struct timeval tv = (struct timeval){0, 0}; // poll
    int act = select(tfd + 1, &temp, NULL, NULL, &tv);
    if (act < 0) {
      if (errno == EINTR && stop)
        break;
      if (errno == EINTR)
        continue;
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

        char addrstr[INET6_ADDRSTRLEN];
        addrstr[0] = '\0';
        if (res->ai_family == AF_INET) {
          struct sockaddr_in *s4 = (struct sockaddr_in *)res->ai_addr;
          inet_ntop(AF_INET, &s4->sin_addr, addrstr, sizeof(addrstr));
        } else if (res->ai_family == AF_INET6) {
          struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)res->ai_addr;
          inet_ntop(AF_INET6, &s6->sin6_addr, addrstr, sizeof(addrstr));
        }

        if (addrstr[0] == '\0') {
          send(i, "NOTFOUND", 8, MSG_NOSIGNAL);
        } else {
          char ok[256];
          int length = snprintf(ok, sizeof(ok), "OK\t%s", addrstr);
          if (length > 0)
            send(i, ok, (size_t)length, MSG_NOSIGNAL);
        }
        freeaddrinfo(res);
      }
    }
  }

  // Graceful shutdown
  for (int i = 0; i <= tfd; i++) {
    if (i != sfd && FD_ISSET(i, &fds))
      close(i);
  }
  close(sfd);
  unlink(argv[1]);
  return 0;
}
