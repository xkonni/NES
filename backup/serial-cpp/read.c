#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 16

int read_port(void) {
  // int fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY | O_NDELAY );
  int fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY );
  if (fd == -1) {
    /* Could not open the port. */
    perror("open_port: Unable to open /dev/ttyUSB0 - ");
  }

  sleep(1);
  char buffer[BUFFER_SIZE];
  int n = read(fd, buffer, sizeof(buffer));
  if (n < 0) {
    printf("read failed!\n", stderr);
  } else {
    printf("great success!\n");
  }
  return fd;
}

int main(void) {
  int fd = read_port();
  char buffer[BUFFER_SIZE];

  sleep(1);
  int i;
  for (i = 0; i < 10; i++) {
    int n = read(fd, buffer, sizeof(buffer));
    printf("read %d bytes: %s\n", n, buffer);
    usleep(1000);
  }

  return 0;
}
