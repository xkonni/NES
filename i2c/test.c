#include "i2cfunc.h"

int main(void) {
  int i2cbus;
  char *filename = "/dev/i2c-1";

  i2cbus = i2c_open(filename);

  i2c_connect(i2cbus, 0x19);

  unsigned char buf[10] = {0};
  unsigned char reg[] = {0x0f, 0x10, 0x11, 0x12};
  int r;
  for (r = 0; r < sizeof(reg)/sizeof(unsigned char); r++) {
    printf("reg: %#04x\n", reg[r]);
    // Write register address
    i2c_write(i2cbus, &reg[r], 1);
    // read register
    i2c_read(i2cbus, &buf, 1);

  }
}
