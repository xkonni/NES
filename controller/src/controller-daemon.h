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

    /** @struct sensors
      * @brief sensors struct
      * @details
      *   holds angles of both sensors
      */
    struct sensors {
      double theta1;          ///< sensor1 theta angle
      double phi1;            ///< sensor1 phi angle
      double theta2;          ///< sensor2 theta angle
      double phi2;            ///< sensor2 phi angle
      /**
       * @fn sensor()
       * @brief default constructor for the sensor struct
       */
      sensors() {
        theta1        = 0;
        phi1          = 0;
        theta2        = 0;
        phi2          = 0;
      }
    };

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

    /** @fn void socket_write_sensorcommand (int sensor,
     *          messages::sensorcommand *command, messages::sensordata *data);
     * @brief send sensorcommand
     *
     * @param[in]  sensor   sensor to send to
     * @param[in]  command  the command to send
     * @param[out] data     the data reply
     */
    void socket_write_sensorcommand (int sensor,
        messages::sensorcommand *command, messages::sensordata *data);

    /// @var sockfd listening socket
    int sockfd;

    /// @var currentSensors latest readings
    sensors currentSensors;
};

#endif
