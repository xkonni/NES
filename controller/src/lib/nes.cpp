/**
  * @file nes.cpp
  * @brief parameters and helper functions
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "nes.h"

void print_error(const char *reply) {
  perror(reply);
  exit(1);
}

void print_motorcommand (char inout, messages::motorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][MOTOR  | COMMAND | ID %d] %s",
      inout, command->motor(),
      messages::motorcommand::commandType_Name(command->type()).c_str());
  if(command->has_steps())
    sprintf(msg, "%s steps: %d", msg, command->steps());
  if(command->has_acc())
    sprintf(msg, "%s acc: %d", msg, command->acc());
  sprintf(msg, "%s [%d byte]\n", msg, command->ByteSize());
  printf("%s", msg);
}

void print_motorstatus (char inout, messages::motorstatus *status) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][MOTOR  | STATUS  | ID %d] pos: %d [%d byte]\n",
      inout, status->motor(), status->pos(), status->ByteSize());
  printf("%s", msg);
}

void print_sensorcommand (char inout, messages::sensorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | COMMAND | ID %d] %s [%d byte]\n",
      inout, command->sensor(),
      messages::sensorcommand::commandType_Name(command->type()).c_str(),
      command->ByteSize());
  printf("%s", msg);
}

void print_sensordata(char inout, messages::sensordata *data) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | DATA    | ID %d] theta: %d, phi: %d [%d bytes]\n",
      inout, data->sensor(), data->theta(), data->phi(), data->ByteSize());
  printf("%s", msg);
}

void convert_coordinates(int x, int y, int z, int *theta, int *phi) {
  double r;
  double d_theta;
  double d_phi;

  int tmp_x = x;
  int tmp_y = y;
  int tmp_z = z;

  /*
   * MAGIC
   * rotate the system
   */
  x = tmp_y;
  y = tmp_z;
  z = tmp_x;

  // radius
  r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

  /*
   * angle phi [0; 2pi]
   * see https://de.wikipedia.org/wiki/Kugelkoordinaten#Umrechnungen
   */
  // first case
  if (x > 0) {
    // printf("first case\n");
    d_phi = atan(y/x);
  }
  // second case
  else if (x == 0) {
    // printf("second case\n");
    // sgn function
    if (y < 0) d_phi = -1 * M_PI_2;
    else if (y == 0) d_phi = 0;
    else d_phi = 1 * M_PI_2;
  }
  // third case
  else if (( x < 0) && (y >= 0)) {
    // printf("third case\n");
    d_phi = atan(y/x) + M_PI;
  }
  // fourth case
  else if (( x < 0) && (y < 0)) {
    // printf("fourth case\n");
    d_phi = atan(y/x) - M_PI;
  }
  else
    printf("this should never happen!\n");

  /*
   * angle theta [0; pi]
   */
  if (r != 0) d_theta = acos(z/r);
  else d_theta = 0;

  // convert to degrees
  int pi_to_deg = 180/M_PI;
  *theta = (int) (d_theta * pi_to_deg);
  *phi = (int) (d_phi * pi_to_deg);

  // DEBUG
  // printf("x: %5d, y: %5d, z: %5d -> r: %4.2f, theta: %3.2f / %4d, phi: %3.2f / %4d\n", x, y, z, r,
  //     d_theta, *theta, d_phi, *phi);
}
