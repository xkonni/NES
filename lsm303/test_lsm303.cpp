#include <LSM303.h>
#include <iostream>
#include <Wire.h>

int main(int argc, char** argv)
{
  std::cout << "test_lsm303" << std::endl;

  LSM303 lsm;
  lsm.init();
  lsm.enableDefault();
  //lsm.read();
  Wire.beginTransmission(0x19);
  std::cout << "heading: " << lsm.heading() << std::endl;

  std::cout << "done." << std::endl;

  return 0;
}

