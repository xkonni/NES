/*
 * daemon.h
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#include "BBBiolib.h"

#define PORT		2020

#define BUFFERSIZE	256

#define MIN_POS		-70
#define MAX_POS		70

#define GPIO_HOLD	10
#define GPIO_TIMEOUT	90

typedef struct {
  int header; int step; int dir; int pos;
} motor; 

motor motor1 = { 8, 11, 12, 0 };
motor motor2 = { 8, 13, 14, 0 };

void error(const char *reply);
int motor_turn (int steps);
int socket_write (int sock, char *buf);
int socket_read (int sock, char *buf);
