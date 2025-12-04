#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT "80" // the port number that we have to use.
#define BACKLOG 5 // max number of connections requests to queue

// generic structure for IPV4:

// struct sockaddr_in {
//     sa_family_t sin_family; // AF_INET
//     in_port_t sin_port;     // Port num
//     struct in_addr sin_addr; // IPV4 Addr
// };

void stripper(char *str) {
  // strip the CLRF
  if (str == NULL) {
    return;
  }
  size_t len = strlen(str);

  // strip out the trailing chars
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
    len--;
  }
  if (len > 0 && str[len - 1] == '\r') {
    str[len - 1] = '\0';
    len--;
  }
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; i += 1) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  /*
   * GET /file/path HTTP/1.1
   * Host: host.name.com
   * Connection: close
   * */

  // create char pointers to filepath, port, and connection type
  char *hn = NULL;
  char *port = NULL;
  char *connection = NULL;
  // read command line arguments and parse the CRLF
  size_t len = 0;
  ssize_t read;

  read = getline(&hn, &len, stdin);
  if (read == -1) {
    perror("getline");
    return 1;
  }
  read = getline(&port, &len, stdin);
  if (read == -1) {
    perror("getline");
    return 1;
  }
  read = getline(&connection, &len, stdin);
  if (read == -1) {
    perror("getline");
    return 1;
  }

  // strip the file CLRF
  stripper(hn);
  stripper(port);
  stripper(connection);
  printf("Stripping of CLRF Successful\n");

  char *filepath = NULL;
  char *host = NULL;
  char *connection_type = NULL;

  // now parse
  strtok(hn, " ");
  filepath = strtok(NULL, " ");
  host = strchr(port, ':');
  if (host != NULL) {
    host += 2;
  } // advances pointer past ":"
  connection_type = strchr(connection, ':');
  if (connection != NULL) {
    connection_type += 2;
  }

  printf("Now properly parsed out command line arguments\n");
  printf("Filepath: %s\n", filepath);
  printf("Host: %s\n", host);
  printf("Connection Type: %s\n", connection_type);

  // create the socket attributes for sockadd_in
  int sfd, rv;
  struct addrinfo hints = {0};
  struct addrinfo *servinfo, *p;
  memset(&hints, 0, sizeof(hints));

  printf("Now attempting to connect\n");

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  rv = getaddrinfo(host, PORT, &hints, &servinfo);
  if (rv != 0) {
    fprintf(stderr, "Shit is wrong %s \n", gai_strerror(rv));
    goto cleanup;
  }

  for (p = servinfo; p != NULL; p->ai_next) {
    sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (-1 == sfd) {
      perror("socket");
      continue;
    }

    if (connect(sfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sfd);
      perror("connect");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "Client failed to connect: %s\n", host);
    rv = 1;
    goto cleanup;
  }
  close(sfd);
  // fix and send http request to server with a send command

cleanup:
  freeaddrinfo(servinfo);
  // cleanup since getline uses malloc - dynamic
  free(hn);
  free(port);
  free(connection);
  return 0;
}
