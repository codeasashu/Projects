#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MYPORT "3496"

void handleData(int fd) {
  char msg[1024];
  recv(fd, msg, sizeof(msg), 0);
  printf("client said: %s", msg);
  printf("Sleeping for 1 seconds...\n");
  sleep(1);
  char *response = {"OK"};
  write(fd, response, strlen(response));
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

  while (1) {
    printf("accepting connection\n");
    int newfd =
        accept(socketfd, (struct sockaddr *)&their_addr, &their_addr_size);
    if (newfd < 0) {
      continue;
    }
    handleData(newfd);
    close(newfd);
  }

  // Server is listening, now accept client request
  return 0;
}
