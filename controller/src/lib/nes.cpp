#include "nes.h"
/*
 * print error and exit
 */
void print_error(const char *reply) {
  perror(reply);
  exit(1);
}

/*
 * print motorcommand
 */
void print_motorcommand (char inout, messages::motorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  // TODO
  // sprintf(msg, "  [SENSOR | COMMAND | ID %d] %s\n",
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

/*
 * print motorstatus
 */
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

/*
 * print sensorcommand
 */
void print_sensorcommand (char inout, messages::sensorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);
  sprintf(msg, "[%c][SENSOR | COMMAND | ID %d] %s\n",
      inout, command->sensor(),
      messages::sensorcommand::commandType_Name(command->type()).c_str());
  printf(msg);
}

/*
 * print sensordata
 */
void print_sensordata(char inout, messages::sensordata *data) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[%c][SENSOR | DATA    | ID %d] x: %d, y: %d, z: %d\n",
      inout, data->sensor(), data->x(), data->y(), data->z());
  printf(msg);
}
