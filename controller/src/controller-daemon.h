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

class Controller {
  public:

    /**
      * @fn Controller();
      * @brief the constructor for a Controller object
      */
    Controller();

    /**
      * @fn ~Controller();
      * @brief the destructor for a Controller object
      */
    ~Controller();

    /** @fn void calculate_movement (messages::sensordata *data1,
     *          messages::sensordata *data2);
     *  @brief calculate the necessary movement based on reading from two sensors
     *
     *  @param[in]  data1     data from first sensor
     *  @param[in]  data2     data from second sensor
     *  @param[out] command1  first motorcommand resulting from the data
     *  @param[out] command2  second motorcommand resulting from the data
     */
    void calculate_movement (
      messages::sensordata *data1, messages::sensordata *data2,
      messages::motorcommand *command1, messages::motorcommand *command2);

    /** @fn int deg2steps(int deg);
     *  @brief calculate steps from degrees
     *
     *  @param[int]   deg   degrees to turn
     */
    int deg2steps(int deg);

    /** @fn void Controller::socket_write_motorcommand (
      *         messages::motorcommand *command, messages::motorstatus *status) {
      * @brief send motorcommand
      *
      * @param[in]  command   the command to send
      * @param[out] status    the status reply
      */
    void socket_write_motorcommand (
        messages::motorcommand *command, messages::motorstatus *status);

    /** @fn void socket_write_sensorcommand (messages::sensorcommand *command);
      * @brief send sensorcommand
      *
      * @param[in]  command the command to send
      * @param[out] data    the data reply
      */
    void socket_write_sensorcommand (
        messages::sensorcommand *command, messages::sensordata *data);
};

#endif
