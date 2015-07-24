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
#include "nes.h"
#ifdef BBB_CAN
  #include "nes-can.h"
#else
  #include "nes-socket.h"
#endif

#ifdef BBB_HOST
#include "LSM303.h"
#include "I2CBus.h"
#endif

#define NUM_SAMPLES 10
#define SAMPLE_TIMEOUT 100000

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
      * @brief sensor struct
      * @details
      *   describes the angles theta and phi
      *   resulting from the x, y, z sensor values
      *   as well as offsets due to calibration
      */
    struct sensor {
      int id;                 ///< sensor id
      int theta;              ///< current theta angle
      int phi;                ///< current phi angle
      int theta_offset;       ///< calibrated theta offset
      int phi_offset;         ///< calibrated phi offset
      /**
       * @fn sensor()
       * @brief default constructor for the sensor struct
       */
      sensor() {
        id            = 1;
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

    /** @fn         int get_sensordatabuffer (char *buffer);
     * @brief       get buffer containing latest sensordata
     *
     * @param[out]  buffer    buffer to hold the data
     */
    int get_sensordatabuffer (char *buffer);

    /** @fn         int sample(int sample_idx, int *samples);
     * @brief       update sensordata
     *
     * @param[in]   sample_idx  current index
     * @param[out]  samples     array containing up to 10 pairs of samples
     */
    int sample(int sample_idx, int *samples);

    /// @var sensor1 first LSM303DLHC sensor
    sensor sensor1;
    /// @var sockfd listening socket
    int sockfd;
};

#endif
