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
#define MIN_POS       -3600
#define MAX_POS       3600
#define GPIO_HOLD     10
#define GPIO_TIMEOUT  240

typedef struct {
  int header;
  int step;
  int dir;
  int pos;
} motor; 

// int ramp[] = {100, 75, 50, 40, 35, 30, 26, 25, 24, 23, 22, 21, 19, 18, 16, 15, 13, 10, 7, 1};
int ramp[] = {
  100,  71,  53,  42,  33,  27,  23,  19,  16,  14,
   12,  11,  10,   9,   8,   7,   6,   6,   5,   5,
    5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1
};
int rampN = 50;

motor motor1 = { 8, 11, 12, 0 };
motor motor2 = { 8, 13, 14, 0 };

void error(const char *reply);
void motor_step(motor *m, int timeout);
void motor_dir(motor *m, int dir);
void motor_loop (motor *m, int steps, int acc);
void socket_write (int sockfd, char *msg);
int socket_read (int sockfd);
