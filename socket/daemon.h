/*
 * daemon.h
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#include "BBBiolib.h"

#define BUFFERSIZE 256
#define PORT 2020
#define MIN_POS -70
#define MAX_POS 70

void error(const char *reply);
int motor_turn (int dir, int steps);
int socket_write (int sock, char *buf);
int socket_read (int sock, char *buf);
