#include "stdio.h"
#include "unistd.h"
#include <poll.h>
#include <string.h>
#include <sys/poll.h>

int main(int argc, char *argv[]) {
  char buff[100];
  int r;
  struct pollfd mypoll;
  memset(&mypoll, 0, sizeof(mypoll));
  mypoll.fd = 0;
  mypoll.events = POLLIN;
  printf("Enter your name \n");
  int counter = 0;
  while (1) {
    r = poll(&mypoll, 1, 100);
    if (r == 1) {
      read(0, buff, sizeof(buff));
      break;
    } else {
      counter++;
    }
  }
  printf("poll result: %d \n", counter * 100);
  printf("Hello %s", buff);
  return 0;
}
