#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>


int i2c_open(char *busname);
int i2c_connect(int bus, int device_addr);
int i2c_write(int bus, unsigned char *buf, int length);
int i2c_write_byte(int bus, unsigned char val);
int i2c_read(int bus, unsigned char *buf, int length);
int i2c_read_byte(int bus, unsigned char *val);
int i2c_close(int bus);
