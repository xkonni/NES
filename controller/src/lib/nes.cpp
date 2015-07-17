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
  sprintf(msg, "%s\n", msg);
  printf(msg);
}

void print_motorstatus (char inout, messages::motorstatus *status) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  int i;
  for (i = 0; i < status->motor_size(); i++) {
    messages::motorstatus::motorStatusMsg *motor;
    motor = status->mutable_motor(i);
    sprintf(msg, "%s[%c][MOTOR  | STATUS  | ID %d] pos: %d\n",
        msg, inout, motor->id(), motor->pos());
  }
  printf(msg);
}

void print_sensorcommand (char inout, messages::sensorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);
  sprintf(msg, "[%c][SENSOR | COMMAND | ID %d] %s\n",
      inout, command->sensor(),
      messages::sensorcommand::commandType_Name(command->type()).c_str());
  printf(msg);
}

void print_sensordata(char inout, messages::sensordata *data) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | DATA    | ID %d] theta: %f, phi: %f\n",
      inout, data->sensor(), data->theta(), data->phi());
  printf(msg);
}

void convert_coordinates(int x, int y, int z, double *theta, double *phi) {
  double r;

  // radius
  r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  // angle theta [0; pi]
  if (r != 0) *theta = acos(z/r)/M_PI*180;
  else *theta = 0;
  // angle phi [0; 2pi]
  *phi = atan2(y, x)/M_PI*180;
}