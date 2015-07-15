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
void print_motorcommand (messages::motorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[COMMAND: %7s]",
      messages::motorcommand::commandType_Name(command->type()).c_str());
  if(command->has_motor())
    sprintf(msg, "%s motor: %d", msg, command->motor());
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
void print_motorstatus (messages::motorstatus *status) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  int i;
  for (i = 0; i < status->motor_size(); i++) {
    messages::motorstatus::motorStatusMsg *motor;
    motor = status->mutable_motor(i);
    sprintf(msg, "%s[STATUS: motor: %d] pos: %d\n", msg, motor->id(), motor->pos());
  }
  printf(msg);
}

/*
 * print sensorcommand
 */
void print_sensorcommand (messages::sensorcommand *command) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);
  sprintf(msg, "[COMMAND: SENSOR %d] %s\n",
      command->sensor(),
      messages::sensorcommand::commandType_Name(command->type()).c_str());
  printf(msg);
}

/*
 * print sensordata
 */
void print_sensordata(messages::sensordata *data) {
  char *msg = (char *) malloc(BUFFERSIZE*sizeof(char));
  bzero(msg, BUFFERSIZE);

  sprintf(msg, "[DATA:    SENSOR %d] x: %d, y: %d, z: %d\n",
      data->sensor(), data->x(), data->y(), data->z());
  printf(msg);
}
