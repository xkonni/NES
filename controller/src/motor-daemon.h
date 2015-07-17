/**
  * @file motor-daemon.h
  * @brief receive protobuf messages containing motorcommands, use them to control
  *   two step motors
  *
  * @details
  *   This file handles multiple motors.
  *   When receiving a motorcommand, it controls the motors accordingly.
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#ifndef __MOTOR_DAEMON_H
#define __MOTOR_DAEMON_H

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
#include "nes-socket.h"

#ifdef HOST_BBB
#undef HZ
#include "BBBiolib.h"
#endif

#define GPIO_HOLD         20
#define GPIO_TIMEOUT      480

class Motor {
  public:
    /**
      * @fn Motor();
      * @brief the constructor for a Motor object
      */
    Motor();
    /**
      * @fn ~Motor();
      * @brief the destructor for a Motor object
      */
    ~Motor();

    /** @struct motor
      * @brief motor struct
      * @details
      *   describes the header and step/direction pins
      *   the motor is connected to as well as the current
      *   position and limits. a complete turn-around counts
      *   800 steps
      */
    struct motor {
      int header;    ///< the pin header
      int step;      ///< pin for the step signal
      int dir;       ///< pin for the direction signal
      int pos;       ///< current position
      int minpos;    ///< minimum position
      int maxpos;    ///< maximum position

      /**
       * @fn motor(int header_in, int step_in, int dir_in, int pos_in,
                int minpos_in, int maxpos_in)
       * @brief constructor for the motor struct
       * @param[in] header    initial value for the pin header
       * @param[in] step      initial value for pin for the step signal
       * @param[in] dir       initial value for pin for the direction signal
       * @param[in] pos       initial value for current position
       * @param[in] minpos    initial value for minimum position
       * @param[in] maxpos    initial value for maximum position
       */
      motor(int header_in, int step_in, int dir_in, int pos_in,
          int minpos_in, int maxpos_in) {
        header  = header_in;
        step    = step_in;
        dir     = dir_in;
        pos     = pos_in;
        minpos  = minpos_in;
        maxpos  = maxpos_in;
      };

      /**
       * @fn motor()
       *
       * @brief default constructor for the motor struct
       */
      motor() {
        header  = 0;
        step    = 0;
        dir     = 0;
        pos     = 0;
        minpos  = 0;
        maxpos  = 0;
      }
    };

    /** @var ramp values used for acceleration and
      * deceleration of the motors
      */
    const int ramp[40];
    // = {
    //   100,  71,  53,  42,  33,  27,  23,  19,  16,  14,
    //    12,  11,  10,   9,   8,   7,   6,   6,   5,   5,
    //     5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
    //     2,   2,   2,   2,   2,   2,   2,   2,   2,   2
    // };

    /**
     * @var rampN number of ramp elements
     */
    static const int rampN = 40;

    /** @fn         void handle_motorcommand (messages::motorcommand *command,
      *                 messages::motorstatus *status)
      *@brief       handle a motorcommand
      *
      * @param[in]  command the motorcommand to handle
      * @param[out] status  the motorstatus reply
      */
    void handle_motorcommand (messages::motorcommand *command, messages::motorstatus *status);

    /** @fn void motor_step (motor *m, int timeout);
      * @brief do a single step on the motor
      *
      * @param[in] m        the motor
      * @param[in] timeout  time to hold the high value on the pin
      */
    void motor_step (motor *m, int timeout);

    /** @fn         void motor_dir (motor *m, int dir);
      * @brief      change the motor direction
      *
      * @param[in]  m       the motor
      * @param[in]  dir     the desired direction
      */
    void motor_dir (motor *m, int dir);

    /** @fn         void motor_loop (motor *m, int steps, int acc);
      * @brief      do some steps on the motor
      *
      * @param[in]  m the   motor
      * @param[in]  steps   number of steps
      * @param[in]  acc     acceleration used [1..10]
      */
    void motor_loop (motor *m, int steps, int acc);

    /** @fn         void socket_read_motorcommand (int sockfd);
      * @brief      read a motorcommand from the socket
      */
    void socket_read_motorcommand ();

    /** @fn         void socket_write_motorstatus (int sockfd, messages::motorstatus *status);
      * @brief write motorstatus to the socket
      *
      * @param[in]  sockfd  the socket
      * @param[in]  data    motorstatus to be written
      */
    void socket_write_motorstatus (int sockfd, messages::motorstatus *status);

    /// @var motor1 first connected motor
    motor motor1;
    /// @var motor2 second connected motor
    motor motor2;
    /// @var sockfd listening socket
    int sockfd;
};

#endif
