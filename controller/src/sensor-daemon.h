/**
  * @file sensor-daemon.h
  * @brief read sensor values, send them via protobuf messages
  *
  * @details
  *   This file handles multiple sensors.
  *   It periodically reads their values and stores them.
  *   When receiving a command, it sends the latest sensor values.
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#ifndef __SENSOR_DAEMON_H
#define __SENSOR_DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "messages.pb.h"
#include "nes-socket.h"
#include "nes.h"

#ifdef HOST_BBB
#include "LSM303.h"
#include "I2CBus.h"
#endif

class Sensor {
  public:
    /**
      * @fn Sensor();
      * @brief the constructor for a Sensor object
      */
    Sensor();

    /**
      * @fn ~Sensor();
      * @brief the destructor for a Sensor object
      */
    ~Sensor();

    /** @struct sensor
      * @brief sensor typedef
      * @details
      *   describes the angles theta and phi
      *   resulting from the x, y, z sensor values
      *   as well as offsets due to calibration
      */
    struct sensor {
      double theta;           ///< current theta angle
      double phi;             ///< current phi angle
      double theta_offset;    ///< calibrated theta offset
      double phi_offset;      ///< calibrated phi offset
      /**
       * @fn sensor()
       * @brief default constructor for the sensor struct
       */
      sensor() {
        theta         = 0;
        phi           = 0;
        theta_offset  = 0;
        phi_offset    = 0;
      }
    };

    /** @fn         void handle_sensorcommand (messages::sensorcommand *command,
      *                 messages::sensordata *data);
      * @brief      handle a sensorcommand
      *
      * @param[in]  command   the sensorcommand to handle
      * @param[out] data      the sensordata reply
      */
    void handle_sensorcommand (messages::sensorcommand *command, messages::sensordata *data);

    /** @fn   void socket_read_sensorcommand (int sockfd);
      * @brief read a sensorcommand from the socket
      */
    void socket_read_sensorcommand ();

    /** @fn         void socket_write_sensordata (int sockfd, messages::sensordata *data);
      * @brief      write sensordata to the socket
      *
      * @param[in]  sockfd    the socket
      * @param[in]  data      sensordata to be written
      */
    void socket_write_sensordata (int sockfd, messages::sensordata *data);

    /// @var sensor1 first LSM303DLHC sensor
    sensor sensor1;
    /// @var sensor2 second LSM303DLHC sensor
    sensor sensor2;
    /// @var sockfd listening socket
    int sockfd;
};

#endif
