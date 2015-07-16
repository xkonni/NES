#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "messages.pb.h"

#define BUFFERSIZE        256

#define CONTROLLER_PORT   2020
#define SENSOR_PORT       2021
// #define SENSOR_HOST       "localhost"
#define SENSOR_HOST        "192.168.21.37"
#define MOTOR_PORT        2022
// #define MOTOR_HOST        "localhost"
#define MOTOR_HOST        "192.168.21.37"

#define NET_IN            '<'
#define NET_OUT           '>'

void print_error(const char *reply);
void print_motorcommand(char inout, messages::motorcommand *command);
void print_motorstatus(char inout, messages::motorstatus *status);
void print_sensorcommand (char inout, messages::sensorcommand *command);
void print_sensordata(char inout, messages::sensordata *data);
void convert_sensordata(messages::sensordata *data, std::vector<double> *scoord);
