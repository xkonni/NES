#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int main(void) {
  int file;
  char *filename = "/dev/i2c-1";
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the i2c bus");
    exit(1);
  }

  int addr = 0x19;     // The I2C address of the device
  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }

  unsigned char buf[10] = {0};

  unsigned char reg[] = {0x0f, 0x10, 0x11, 0x12};
  int r;
  for (r = 0; r < sizeof(reg)/sizeof(unsigned char); r++) {
    buf[0] = reg[r];
    // Write address
    printf("reg: %#04x\n", reg[r]);
    if (write(file, buf, 1) != 1) {
      /* ERROR HANDLING: i2c transaction failed */
      printf("Failed to write to the i2c bus: %s.\n", strerror(errno));
      printf("\n\n");
    }

    // Using I2C Read
    if (read(file,buf,2) != 2) {
      /* ERROR HANDLING: i2c transaction failed */
      printf("Failed to read from the i2c bus: %s.\n", strerror(errno));
      printf("\n\n");
    } else {
    /* Device specific stuff here */
      printf("reg[%d] %#04x %#04x\n", r, reg[r], *buf);
    }
  }
}
