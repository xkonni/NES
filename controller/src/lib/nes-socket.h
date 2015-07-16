/**
  * @file nes-socket.h
  * @brief handle sockets
  *
  * @details
  *   This file handles sockets
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
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

/** @fn         int socket_connect(int port, const char *hostname);
  * @brief      connect to a host:port
  *
  * @param[in]  port      the port to use
  * @param[in]  hostname  the host to connect to
  */
int socket_connect(int port, const char *hostname);

/** @fn         int socket_open(int port);
  * @brief      open a listening socket
  *
  * @param[in]  port      the port to listen on
  */
int socket_open(int port);

/** @fn         void socket_setnonblock(int sockfd);
  * @brief      set listening socket to non-blocking
  *
  * @param[in]  sockfd    the socket to modify
  */
void socket_setnonblock(int sockfd);

#endif
