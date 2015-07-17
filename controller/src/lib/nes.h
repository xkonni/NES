/**
  * @file nes.h
  * @brief parameters and helper functions
  *
  * This file contains some parameter definitions
  * and helper functions
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#ifndef __NES_H
#define __NES_H

#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "messages.pb.h"

#define BUFFERSIZE        256

#define CONTROLLER_PORT   2020
#define SENSOR_PORT       2021
#define SENSOR_HOST       "localhost"
// #define SENSOR_HOST        "192.168.21.37"
#define MOTOR_PORT        2022
#define MOTOR_HOST        "localhost"
// #define MOTOR_HOST        "192.168.21.37"

#define NET_IN            '<'
#define NET_OUT           '>'

/** @fn         void print_error(const char *reply);
  * @brief      print message, error and exit
  *
  * @param[in]  reply     the message
  */
void print_error(const char *reply);

/** @fn         void print_motorcommand(char inout, messages::motorcommand *command);
  * @brief      show the motorcommand in a readable way
  *
  * @param[in]  inout     character to show before the message
  * @param[in]  command   the motorcommand to show
  */
void print_motorcommand(char inout, messages::motorcommand *command);

/** @fn         void print_motorstatus(char inout, messages::motorstatus *status);
  * @brief      show the motorstatus in a readable way
  *
  * @param[in]  inout     character to show before the message
  * @param[in]  status    the motorstatus to show
  */
void print_motorstatus(char inout, messages::motorstatus *status);

/** @fn         void print_sensorcommand (char inout, messages::sensorcommand *command);
  * @brief      show the sensorcommand in a readable way
  * @param[in]  inout     character to show before the message
  * @param[in]  command   the sensorcommand to show
  */
void print_sensorcommand (char inout, messages::sensorcommand *command);

/** @brief show the sensordata in a readable way
  * @param[in] inout character to show before the message
  * @param[in] data the sensordata to show
  */
void print_sensordata(char inout, messages::sensordata *data);

/** @fn         void convert_coordinates(int x, int y, int z, double *theta, double *phi);
  * @brief      convert xyz coordinates into spherical coordinates
  *
  * @param[in]  x         the x-coordinate
  * @param[in]  y         the y-coordinate
  * @param[in]  z         the z-coordinate
  * @param[out] theta     the theta angle [0; pi]
  * @param[out] phi       the phi angle [0; 2pi]
  */
void convert_coordinates(int x, int y, int z, double *theta, double *phi);

#endif