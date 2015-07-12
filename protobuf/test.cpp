#include <stdio.h>
#include <iostream>
#include "sensor.pb.h"
#include <string>

int main(void) {
  sensor::sensorData data;
  data.set_x(1);
  data.set_y(2);
  data.set_z(3);
  printf("x: %d, y: %d, z: %d\n", data.x(), data.y(), data.z());
  return(0);
}
