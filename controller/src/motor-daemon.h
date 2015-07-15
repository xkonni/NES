/*
 * daemon.h
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "messages.pb.h"

#ifdef HOST_BBB
#undef HZ
#include "BBBiolib.h"
#endif

#define CONTROLLER_PORT   2020
#define SENSOR_PORT       2021
#define MOTOR_PORT        2022
#define BUFFERSIZE        256
#define GPIO_HOLD         20
#define GPIO_TIMEOUT      480

typedef struct {
  int header;
  int step;
  int dir;
  int pos;
  int minpos; // an entire turn
  int maxpos; // counts 800 microsteps
} motor;
motor motor1, motor2;

int ramp[] = {
  100,  71,  53,  42,  33,  27,  23,  19,  16,  14,
   12,  11,  10,   9,   8,   7,   6,   6,   5,   5,
    5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2
};
int rampN = 40;

void print_error (const char *reply);
void print_motorcommand (messages::motorcommand *command);
void print_motorstatus (messages::motorstatus *status);
messages::motorstatus* handle_motorcommand (messages::motorcommand *command);
void motor_step (motor *m, int timeout);
void motor_dir (motor *m, int dir);
void motor_loop (motor *m, int steps, int acc);
int socket_open();
void socket_read (int sockfd);
void socket_setnonblock(int sockfd);
void socket_write (int sockfd, messages::motorstatus *response);
