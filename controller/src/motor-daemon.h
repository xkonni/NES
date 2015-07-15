/*
 * motor-daemon.h
 *
 * daemon to control the stepper motor controller
 * via a socket, using protobuf messages
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
#include "nes.h"
#include "lib/nes-socket.h"

#ifdef HOST_BBB
#undef HZ
#include "BBBiolib.h"
#endif

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
void handle_motorcommand (messages::motorcommand *command, messages::motorstatus *status);
void motor_step (motor *m, int timeout);
void motor_dir (motor *m, int dir);
void motor_loop (motor *m, int steps, int acc);
void socket_read_motorcommand (int sockfd);
