#define MAX_EVENTS 5
#define READ_SIZE 10
#include <poll.h>   // for poll(), struct pollfd
#include <stdio.h>  // for fprintf()
#include <stdlib.h> // for close(), read()
#include <string.h> // for strncmp
#include <unistd.h> // for close(), read()

int main() {
  int running = 1, event_count, i;
  size_t bytes_read;
  char *buff2;
  buff2 = malloc(sizeof(char) * READ_SIZE);
  struct pollfd fds[MAX_EVENTS];

  fds[0].fd = 0; // Standard input (stdin)
  fds[0].events = POLLIN;

  while (running) {
    printf("\nPolling for input...\n");
    event_count = poll(fds, 1, 5000); // 1 file descriptor, 30 second timeout
    if (event_count == -1) {
      fprintf(stderr, "Failed to poll\n");
      return 1;
    }

    printf("%d ready events\n", event_count);
    if (fds[0].revents & POLLIN) {
      printf("Reading file descriptor '%d' -- ", fds[0].fd);
      if ((bytes_read = read(fds[0].fd, buff2, sizeof(buff2) + 1)) == -1) {
        fprintf(stderr, "Failed to read from file descriptor\n");
        return 1;
      }
      printf("%zd bytes read.\n", bytes_read);
      buff2[bytes_read] = '\0';
      printf("Read '%s'\n", buff2);

      if (!strncmp(buff2, "stop\n", 5))
        running = 0;
    }
  }

  return 0;
}
