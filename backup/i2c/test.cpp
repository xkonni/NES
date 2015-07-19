#include "LSM303.h"
#include "I2CBus.h"
#include <stdio.h>
#include <iostream>
#include <vector>

int main(void) {
  printf("foo\n");
  LSM303 mag ("/dev/i2c-1");
  mag.enable();
  while (1) {
    mag.readMag();
    printf("mag.x: %d, mag.y: %d, mag.z: %d\n", mag.m[0], mag.m[1], mag.m[2]);
    usleep(100000);
  }
  return(0);
}
