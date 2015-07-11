#include "i2cfunc.h"

#define I2CBUS            "/dev/i2c-1"
#define LSM303DLH_ID      0x32
#define LSM303DLH_MAG     0x1E
#define LSM303DLH_ACC     0x19
enum lsm303_regAddr {
  DEVICE_ID = 0x0f,

  DLH_OUT_X_H_M     = 0x03,
  DLH_OUT_X_L_M     = 0x04,
  DLH_OUT_Y_H_M     = 0x05,
  DLH_OUT_Y_L_M     = 0x06,
  DLH_OUT_Z_H_M     = 0x07,
  DLH_OUT_Z_L_M     = 0x08,

  CTRL_REG1_A       = 0x20, // DLH, DLM, DLHC
  CTRL_REG2_A       = 0x21, // DLH, DLM, DLHC
  CTRL_REG3_A       = 0x22, // DLH, DLM, DLHC
  CTRL_REG4_A       = 0x23, // DLH, DLM, DLHC
  CTRL_REG5_A       = 0x24, // DLH, DLM, DLHC

  CRA_REG_M         = 0x00, // DLH, DLM, DLHC
  CRB_REG_M         = 0x01, // DLH, DLM, DLHC
  MR_REG_M          = 0x02, // DLH, DLM, DLHC


};

int lsm303_getDevice(int handle, unsigned char *dev);
int lsm303_initMag(int handle);
