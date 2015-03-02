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
#ifdef BBB_CAN
  #include "nes-can.h"
#else
  #include "nes-socket.h"
#endif

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

    /** @fn   int Controller:send_motorcommand(messages::motorcommand *command)
     *  @brief send a motorcommand to the motor-daemon
     *
     *  @param[in]  command   command to send
     */
    int send_motorcommand(messages::motorcommand *command);

    /** @fn   int send_sensorcommand(messages::sensorcommand *command) {
     *  @brief send a sensorcommand to the sensor-daemon
     *
     *  @param[in]  command   command to send
     */
    int send_sensorcommand(messages::sensorcommand *command);

    /** @fn int deg2steps(int deg);
     *  @brief calculate steps from degrees
     *
     *  @param[int]   deg   degrees to turn
     */
    int deg2steps(int deg);

    /// @var sockfd listening socket
    int sockfd;
};

#endif
