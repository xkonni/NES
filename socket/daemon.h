/*
 * daemon.h
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "BBBiolib.h"

#define PORT          2020
#define BUFFERSIZE    256
#define MIN_POS       -400
#define MAX_POS       400
#define GPIO_HOLD     20
#define GPIO_TIMEOUT  480

typedef struct {
  int header;
  int step;
  int dir;
  int pos;
} motor; 

int ramp[] = {
  100,  71,  53,  42,  33,  27,  23,  19,  16,  14,
   12,  11,  10,   9,   8,   7,   6,   6,   5,   5,
    5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2
};
int rampN = 40;

motor motor1 = { 8, 11, 12, 0 };
motor motor2 = { 8, 13, 14, 0 };

void error(const char *reply);
void motor_step(motor *m, int timeout);
void motor_dir(motor *m, int dir);
void motor_loop (motor *m, int steps, int acc);
void socket_write (int sockfd, char *msg);
int socket_read (int sockfd);
