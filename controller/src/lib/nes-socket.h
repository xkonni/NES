/*
 * nes-socket.h
 *
 * functions for sockets
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

int socket_open();
void socket_setnonblock(int sockfd);
void socket_write (int sockfd, messages::motorstatus *response);
