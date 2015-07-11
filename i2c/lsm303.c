#include "lsm303.h"

int main(void) {
  int accHandle, magHandle;

  accHandle = i2c_open(I2CBUS);
  magHandle = i2c_open(I2CBUS);
  i2c_connect(accHandle, LSM303DLH_ACC);
  i2c_connect(magHandle, LSM303DLH_MAG);
  lsm303_initMag(magHandle);
  lsm303_initAcc(accHandle);

  while (1) {
    lsm303_readMag(magHandle);
    usleep(500);
  }
}

int lsm303_getDevice (int handle, unsigned char *dev) {
  unsigned char reg = DEVICE_ID;
  i2c_write(handle, &reg, 1);
  i2c_read(handle, dev, 1);
}
int lsm303_initAcc(int handle) {
  // 0x08 = 0b00001000
  // FS = 00 (+/- 2 g full scale); HR = 1 (high resolution enable)
  i2c_write_byte(handle, CTRL_REG4_A);
  i2c_write_byte(handle, 0x08);

  // 0x47 = 0b01000111
  // ODR = 0100 (50 Hz ODR); LPen = 0 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
  i2c_write_byte(handle, CTRL_REG1_A);
  i2c_write_byte(handle, 0x47);
}

int lsm303_initMag(int handle) {
  // 0x0C = 0b00001100
  // DO = 011 (7.5 Hz ODR)
  i2c_write_byte(handle, CRA_REG_M);
  i2c_write_byte(handle, 0x0C);

  // 0x20 = 0b00100000
  // GN = 001 (+/- 1.3 gauss full scale)
  i2c_write_byte(handle, CRB_REG_M);
  i2c_write_byte(handle, 0x20);

  // 0x00 = 0b00000000
  // MD = 00 (continuous-conversion mode)
  i2c_write_byte(handle, MR_REG_M);
  i2c_write_byte(handle, 0x00);
}

int lsm303_readMag(int handle) {
  unsigned char buf[6];
  int x, y, z;
  // write address of X_HIGH_MAG
  // the rest follows in order
  i2c_write_byte(handle, DLH_OUT_X_H_M);
  // read all values
  int i;
  for (i = 0; i < 6; i++) {
    i2c_read_byte(handle, &buf[i]);
    // printf("buf[%d]: %#04x\n", i, buf[i]);
  }

  x = (buf[0] << 8) | buf[1];
  y = (buf[2] << 8) | buf[3];
  z = (buf[4] << 8) | buf[5];
  printf("x: %#06x, y: %#06x, z: %#06x\n", x, y, z);
}
