#ifdef HOST_BBB
#include "LSM303.h"
#include "I2CBus.h"
#endif
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <vector>

int main(void) {
  printf("foo\n");
#ifdef HOST_BBB
  LSM303 mag ("/dev/i2c-1");
  mag.enable();
#endif
  while (1) {
#ifdef HOST_BBB
    mag.readMag();
    printf("mag.x: %d, mag.y: %d, mag.z: %d\n", mag.m[0], mag.m[1], mag.m[2]);
#endif
    usleep(100000);
  }
  return(0);
}
