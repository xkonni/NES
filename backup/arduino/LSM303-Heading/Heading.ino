#include <Wire.h>
#include <LSM303.h>

LSM303 compass;
char report[80];

float heading, headingx, headingy, headingz;
float prev_heading, prev_headingx, prev_headingy, prev_headingz;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */

  // compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  // compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
  compass.m_min = (LSM303::vector<int16_t>){-149, -596, -568};
  compass.m_max = (LSM303::vector<int16_t>){+174, +124, +327};
}

void loop() {
  compass.read();

  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.

  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.

  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use

    compass.heading((LSM303::vector<int>){0, 0, 1});

  to use the +Z axis as a reference.
  */

  /* prev_heading = heading; */
  /* prev_headingx = headingx; */
  /* prev_headingy = headingy; */
  /* prev_headingz = headingz; */
  heading = compass.heading();
  /* heading = compass.heading((LSM303::vector<int>){1, 1, 0}); */
  /* headingx = compass.heading((LSM303::vector<int>){1, 0, 0}); */
  /* headingy = compass.heading((LSM303::vector<int>){0, 1, 0}); */
  /* headingz = compass.heading((LSM303::vector<int>){0, 0, 1}); */

  /* sprintf(report, "%d", (int)(heading)); */
    /* hx: %d, hy: %d, hz: %d\n", */
    /* (int)(headingx - prev_headingx), */
    /* (int)(headingy - prev_headingy), */
    /* (int)(headingz - prev_headingz) */
  Serial.println(heading);
  delay(100);
}
