#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// This file WILL be graded.

// The Run button will compile both server.c and test.c.
// It will then run this server, and run the tester
// to connect to it.
// The server will be placed into the background and sent
// the SIGINT signal after 2 seconds.
// Your program will be killed after 5 seconds.

bool stop;

void sighandler(int signal)
{
  if(signal == SIGINT)
  {
    stop = true;
  }
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usage: %s <socket file>\n", argv[0]);
    return 1;
  }
  printf("SERVER: bind to socket file '%s'\n", argv[1]);

  struct timeval timeValue;
  timeValue.tv_sec = 1;
  timeValue.tv_usec = 0;
  int srvsock;
  char hello[5] = "HELLO";
  char data[256];
  struct sockaddr_un sun = {0};
  ssize_t bytes;  
  struct addrinfo addrVal = {0};
  struct addrinfo *addrR;

  srvsock = socket(AF_UNIX, SOCK_STREAM, 0);
  sun.sun_family = AF_UNIX;
  strncpy(sun.sun_path, argv[1], sizeof(sun.sun_path) - 1);
  bind(srvsock, (struct sockaddr *)&sun, sizeof(sun));
  listen(srvsock, 5);
  fd_set mainSet;
  fd_set setTemp;
  FD_ZERO(&mainSet);
  FD_SET(srvsock, &mainSet);
  int topfd = srvsock;

  for(;;)
  {
    signal(SIGINT, sighandler);
    if(stop)
    {
      break;
    }
    setTemp = mainSet;
    struct timespec timeToSleep = {0, 100};
    int act = select(topfd + 1, &setTemp, NULL, NULL, &timeValue);
    if(act < 0)
    {
      if(stop)
      {
        break;
      }
      perror("select");
      continue;
    }
    if(act == 0)
    {
      nanosleep(&timeToSleep, NULL);
      continue;
    }

    if(FD_ISSET(srvsock, &setTemp))
    {
      int clientS = accept(srvsock, NULL, NULL);
      if(clientS >= 0)
      {
        send(clientS, hello, 5, MSG_NOSIGNAL);
        fd_set temp;
        FD_ZERO(&temp);
        FD_SET(clientS, &temp);
        struct timeval timeW = {1 , 0};

        int selectv = select(clientS + 1, &temp, NULL, NULL, &timeW);
        if(selectv > 0)
        {
          bytes = recv(clientS, data, sizeof(data) - 1, MSG_NOSIGNAL);
          if(bytes == 5 && strncmp(data, "HELLO", 5) == 0)
          {
            FD_SET(clientS, &mainSet);
            if(clientS > topfd)
            {
              topfd = clientS;
            }
          }
          else
          {
            close(clientS);
          }
        }
        else
        {
          close(clientS);
        }
      }
      continue;
    }

    for(int i = 0; i <= topfd; i++)
    {
      if(i == srvsock)
      {
        continue;
      }
      if(!FD_ISSET(i, &setTemp))
      {
        continue;
      }
      bytes = recv(i, data, sizeof(data) - 1, MSG_NOSIGNAL);
      data[bytes] = '\0';
      if(strncmp(data, "BYE", 3) == 0)
      {
        close(i);
        FD_CLR(i, &mainSet);
        continue;
      }

      if(strncmp(data, "A\t", 2) == 0 || strncmp(data, "AAAA\t", 5) == 0)
      {
        char *pointer = strchr(data, '\t');
        if(!pointer)
        {
          continue;
        }
        pointer++;
        if(data[0] == 'A' && data[1] == '\t')
        {
          addrVal.ai_family = AF_INET;
        }
        else
        {
          addrVal.ai_family = AF_INET6;
        }
        addrVal.ai_socktype = SOCK_STREAM;

        int alook = getaddrinfo(pointer, NULL, &addrVal, &addrR);
        if(alook != 0 || !addrR)
        {
          send(i, "NOTFOUND", 8, MSG_NOSIGNAL);
          continue;
        }
        char stringa[128];
        memset(stringa, 0, sizeof(stringa));

        if(addrR->ai_family == AF_INET)
        {
          struct sockaddr_in *s4 = (struct sockaddr_in *)addrR->ai_addr;
          unsigned char *ipv4 = (unsigned char *)&s4->sin_addr;
          sprintf(stringa, "%03u.%03u.%03u.%03u", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        }
        else if(addrR->ai_family == AF_INET6)
        {
          struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)addrR->ai_addr;
          unsigned short *ipv6 = (unsigned short *)&s6->sin6_addr;
          for(int j = 0; j < 8; j++)
          {
            unsigned short seg = ntohs(ipv6[j]);
            sprintf(stringa + strlen(stringa), "%04x", seg);
            if(j < 7)
            {
              strcat(stringa, ":");
            }
          }
        }
        char ok[256];
        snprintf(ok, sizeof(ok), "OK\t%s", stringa);
        send(i, ok, strlen(ok), MSG_NOSIGNAL);
        freeaddrinfo(addrR);
        addrR = NULL;
      }
    }    
  }    

  close(srvsock);
  unlink(argv[1]);
  return (0);
}
