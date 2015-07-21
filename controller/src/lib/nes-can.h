/**
  * @file nes-can.h
  * @brief handle can sockets
  *
  * @details
  *   This file handles can sockets
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#ifndef __NES_CAN_H
#define __NES_CAN_H

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "nes.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

enum {
  CAN_MOTOCOMMAND,
  CAN_MOTORSTATUS,
  CAN_SENSORCOMMAND,
  CAN_SENSORDATA
};

/** @fn         int can_listen();
  * @brief      listen on a can socket
  */
int can_listen(int sockfd, char *buffer);

/** @fn         int can_write
 * @brief       connect to the can bus and write a buffer
 *
 * @param[in]   sockdf    socket to write to
 * @param[in]   canid     can message id
 * @param[in]   buffer    data to write
 * @param[in]   size      size of the data
 */
int can_write(int sockfd, int canid, const char *buffer, int size);

/** @fn         int can_open();
  * @brief      open a can socket
  */
int can_open();

#endif
