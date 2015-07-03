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

#define PORT					2020
#define BUFFERSIZE		256
#define MIN_POS				-70
#define MAX_POS				70
#define GPIO_HOLD			100
#define GPIO_TIMEOUT	900

typedef struct {
  int header;
	int step;
	int dir;
	int pos;
} motor; 

motor motor1 = { 8, 11, 12, 0 };
motor motor2 = { 8, 13, 14, 0 };

void error(const char *reply);
void motor_step(motor *m);
void motor_dir(motor *m, int dir);
void motor_loop (motor *m, int steps);
void socket_write (int sockfd, char *msg);
int socket_read (int sockfd);
