#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "messages.pb.h"

#define BUFFERSIZE        256

#define CONTROLLER_PORT   2020
#define SENSOR_PORT       2021
#define SENSOR_HOST       "localhost"
#define MOTOR_PORT        2022
#define MOTOR_HOST        "localhost"

void print_error(const char *reply);
void print_motorcommand(messages::motorcommand *command);
void print_motorstatus(messages::motorstatus *status);
void print_sensorcommand (messages::sensorcommand *command);
void print_sensordata(messages::sensordata *data);
