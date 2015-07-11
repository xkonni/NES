#include "i2cfunc.h"

int i2c_open(char *handlename) {
  int handle;
  if ((handle = open(handlename, O_RDWR)) < 0) {
    perror("Failed to open the i2c handle");
    exit(1);
  }
  return handle;
}

int i2c_connect(int handle, int device_addr) {
  if (ioctl(handle, I2C_SLAVE, device_addr) < 0) {
    printf("Failed to acquire handle access and/or talk to slave.\n");
    return(1);
  }
  return(0);
}

int i2c_write(int handle, unsigned char *buf, int length) {
  if (write(handle, buf, length) != length) {
    printf("Failed to write to the i2c handle: %s.\n", strerror(errno));
    return(-1);
  }
  return(length);
}

int i2c_write_byte(int handle, unsigned char val) {
  if (write(handle, &val, 1) != 1) {
    printf("Failed to write to the i2c handle: %s.\n", strerror(errno));
    return(-1);
  }
  return(1);
}

int i2c_read(int handle, unsigned char *buf, int length) {
  if (read(handle, buf, length) != length) {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to read from the i2c handle: %s.\n", strerror(errno));
    return(-1);
  }
  // printf("i2c_read %#04x\n", *buf);
  return(length);
}

int i2c_read_byte(int handle, unsigned char *val) {
  if (read(handle, val, 1) != 1) {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to read from the i2c handle: %s.\n", strerror(errno));
    return(-1);
  }
  // printf("i2c_read %#04x\n", *buf);
  return(1);
}

int i2c_close(int handle) {
  if (close(handle) != 0) {
    printf("Failed to close the i2c handle: %s.\n", strerror(errno));
    return(-1);
  }
  return(0);
}
