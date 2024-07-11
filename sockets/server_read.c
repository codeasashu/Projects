#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MYPORT "3496"

const size_t k_max_msg = 4096;

static int32_t read_full(int fd, char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = read(fd, buf, n);
    if (rv <= 0) {
      return -1; // error, or unexpected EOF
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = send(fd, buf, n, 0);
    if (rv <= 0) {
      return -1; // error
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t one_request(int connfd) {
  // 4 bytes header
  char rbuf[4];
  int errno = 0;
  int32_t err = read_full(connfd, rbuf, 4);
  if (err) {
    if (errno == 0) {
      printf("EOF\n");
    } else {
      printf("read() error\n");
    }
    return err;
  }
  printf("rbuf1=%s\n", rbuf);
  uint32_t len = 0;
  len |= ((uint32_t)rbuf[0] & 0xFF) << 24;
  len |= ((uint32_t)rbuf[1] & 0xFF) << 16;
  len |= ((uint32_t)rbuf[2] & 0xFF) << 8;
  len |= ((uint32_t)rbuf[3] & 0xFF);
  // memcpy(&len, rbuf, 4); // assume little endian
  // len = ntohl(len);

  printf("Received length11: %u\n", len);
  printf("rbuf=%s, len=%d\n", rbuf, len);
  if (len > k_max_msg) {
    printf("too long: %d\n", len);
    return -1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res;

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

    while (1) {
      int32_t err = one_request(clientfd);
      if (err) {
        break;
      }
    }
    close(clientfd);
  }
  freeaddrinfo(res);
  return 0;
}
