#include "Wire.h"
#include <iostream>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>

WireClass Wire;

byte WireClass::read()
{
  ;
}

void WireClass::write(byte value)
{
  ;
}

bool WireClass::available()
{
  ;
}

void WireClass::beginTransmission(address_t address)
{
  int file;
  std::string filename = "/dev/i2c-1";
  if ((file = open(filename.c_str(), O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    std::cerr << "Failed to open the i2c bus" << std::endl;
    exit(1);
  }

  if (ioctl(file, I2C_SLAVE, address) < 0) {
    std::cerr << "Failed to acquire bus access and/or talk to slave." << std::endl;
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}

bool WireClass::endTransmission()
{
  ;
}

void WireClass::requestFrom(address_t address, byte reg)
{
  ;
}


