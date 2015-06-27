#ifndef _CUSTOM_WIRE_H_
#define _CUSTOM_WIRE_H_

#include <LSM303.h>

class WireClass {
public:
  typedef uint8_t address_t;

  byte read();
  void write(byte value);
  bool available();
  void beginTransmission(address_t address);
  bool endTransmission();
  void requestFrom(address_t address, byte reg);
};

extern WireClass Wire;

#endif /* _CUSTOM_WIRE_H_ */

