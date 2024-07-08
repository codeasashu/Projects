#include <asm-generic/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define MYPORT "3496"

void handleData(int fd) {
  char msg[10];
  recv(fd, msg, sizeof(msg), 0);
  printf("client said: %s", msg);
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res;
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size;
  int socketfd;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(NULL, MYPORT, &hints, &res);
  socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (socketfd <= 0) {
    printf("error creating socket()");
  }
  int val = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  bind(socketfd, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  int rv = listen(socketfd, 10);
  if (rv < 0) {
    printf("error listen() %s", strerror(rv));
  }

  // Server is listening, now accept client request
  int newfd =
      accept(socketfd, (struct sockaddr *)&their_addr, &their_addr_size);

  handleData(newfd);
  return 0;
}
