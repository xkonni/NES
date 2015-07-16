/*
 * nes-socket.h
 *
 * functions for sockets
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#ifndef __NES_SOCKET_H
#define __NES_SOCKET_H

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

int socket_connect(int port, const char *hostname);
int socket_open(int port);
void socket_setnonblock(int sockfd);

#endif
