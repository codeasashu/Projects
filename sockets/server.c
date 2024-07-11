#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MYPORT "3496"

time_t rawtime;

void handleData(int fd) {
  char msg[10];
  recv(fd, msg, sizeof(msg), 0);
  printf("(%ld) client said: %s \n", rawtime, msg);
  char buf[] = "OK";
  send(fd, buf, sizeof(buf), 0);
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res;

  int socketfd;
  time(&rawtime);
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
  int rv = listen(socketfd, 2);
  if (rv < 0) {
    printf("error listen() %s", strerror(rv));
  }

  // Print server ip and port
  void *addr;
  char ipstr[INET_ADDRSTRLEN];
  struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
  addr = &(ipv4->sin_addr);
  inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);
  int port = htons(ipv4->sin_port);
  printf("server listening on %s:%d ...\n", ipstr, port);

  while (1) {
    struct sockaddr_in client_addr = {};
    socklen_t client_size = sizeof(client_addr);
    int clientfd =
        accept(socketfd, (struct sockaddr *)&client_addr, &client_size);
    if (clientfd < 0) {
      continue; // error
    }
    handleData(clientfd);
    close(clientfd);
  }
  freeaddrinfo(res);
  return 0;
}
