#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFERSIZE        256
#define CONTROLLER_PORT   2020
#define SENSOR_PORT       2021
#define MOTOR_PORT        2022

void print_error(const char *reply);
