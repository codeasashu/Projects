#include "arpa/inet.h"
#include "netdb.h"
#include "unistd.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define MYPORT "9040"

int main(int argc, char *argv[]) {
  int serverfd, clientfd;
  struct addrinfo hints, *res, clientres;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(NULL, MYPORT, &hints, &res);

  serverfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (serverfd <= 0) {
    printf("error creating socket()");
  }
  int val = 1;
  setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  bind(serverfd, res->ai_addr, res->ai_addrlen);

  listen(serverfd, 10);

  while (1) {
    struct sockaddr_in clientaddr;
    socklen_t client_size = sizeof(clientaddr);
    clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &client_size);
    if (clientfd < 0) {
      continue;
    }
    int running = 1;
    while (running) {
      char buff[101];
      int read_bytes;
      read_bytes = read(clientfd, buff, 100);
      printf("client said: %s", buff);
      buff[read_bytes] = '\0';
      if (!strncmp(buff, "stop", 4)) {
        close(clientfd);
        running = 0;
      }
    }
  }
  return 0;
}
