#include <SerialStream.h>
#include <stdio.h>

using namespace LibSerial ;

const int BUFFER_SIZE = 19;


int main(void) {
  LibSerial::SerialStream arduino1;
  arduino1.Open( "/dev/ttyUSB0", std::ios_base::in | std::ios_base::out);
  arduino1.SetBaudRate(SerialStreamBuf::BAUD_9600);
  arduino1.SetParity(SerialStreamBuf::PARITY_NONE);
  arduino1.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
  arduino1.SetNumOfStopBits(1);

  char buffer[BUFFER_SIZE];
  int i;
  for (i = 1; i < 100; i++) {
    arduino1.read(buffer, BUFFER_SIZE);
    printf("buffer[%d]: %s \n", i, buffer);
    usleep(10000);
  }
  arduino1.Close();
}
