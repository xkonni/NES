/*
 * controller.h
 *
 * control motors and sensors, each running as a separate process,
 * communicate via protobuf messages
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
#include <netdb.h>
#include "messages.pb.h"

#define MOTOR_PORT    2020
#define MOTOR_HOST    "localhost"
#define SENSOR_PORT   2022
#define SENSOR_HOST   "localhost"
#define BUFFERSIZE    256

void error(const char *reply);
int socket_connect(int port, const char *hostname);
int socket_read (int sockfd);
void socket_write_command (int port, const char *host, messages::motorcommand *command);
