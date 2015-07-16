/**
  * @file controller-daemon.h
  * @brief control motor- and sensor-daemon
  *
  * @details
  *   This file handles the sensor-daemon and motor-daemon.
  *   It periodically requests values from the sensor-daemon and sends
  *   commands to the motor-daemon to act accordingly.
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#ifndef __CONTROLLER_DAEMON_H
#define __CONTROLLER_DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "messages.pb.h"
#include "nes.h"
#include "nes-socket.h"

/** @fn void socket_write_motorcommand (messages::motorcommand *command)
  * @brief send motorcommand
  *
  * @param[in] command the command to send
  */
void socket_write_motorcommand (messages::motorcommand *command);

/** @fn void socket_write_sensorcommand (messages::sensorcommand *command);
  * @brief send sensorcommand
  *
  * @param[in] command the command to send
  */
void socket_write_sensorcommand (messages::sensorcommand *command);

#endif
