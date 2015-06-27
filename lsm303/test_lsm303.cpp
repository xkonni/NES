#include <LSM303.h>
#include <iostream>

int main(int argc, char** argv)
{
  std::cout << "test_lsm303" << std::endl;

  LSM303 lsm;
  lsm.init();
  lsm.enableDefault();
  //lsm.read();
  std::cout << "heading: " << lsm.heading() << std::endl;

  std::cout << "done." << std::endl;

  return 0;
}

