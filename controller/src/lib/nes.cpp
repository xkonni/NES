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
  printf(msg);
}

void print_motorstatus (char inout, messages::motorstatus *status) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][MOTOR  | STATUS  | ID %d] pos: %d [%d byte]\n",
      inout, status->motor(), status->pos(), status->ByteSize());
  printf(msg);
}

void print_sensorcommand (char inout, messages::sensorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | COMMAND | ID %d] %s [%d byte]\n",
      inout, command->sensor(),
      messages::sensorcommand::commandType_Name(command->type()).c_str(),
      command->ByteSize());
  printf(msg);
}

void print_sensordata(char inout, messages::sensordata *data) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | DATA    | ID %d] theta: %d, phi: %d [%d bytes]\n",
      inout, data->sensor(), data->theta(), data->phi(), data->ByteSize());
  printf(msg);
}

void convert_coordinates(int x, int y, int z, int *theta, int *phi) {
  double r;

  // radius
  r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  // angle theta [0; pi]
  if (r != 0) *theta = (int) (acos(z/r)/M_PI*180);
  else *theta = 0;
  // angle phi [0; 2pi]
  *phi = (int) (atan2(y, x)/M_PI*180);
}
