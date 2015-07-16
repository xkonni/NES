/*
 * controller-daemon.h
 *
 * control motors and sensors, each running as a separate process,
 * communicate via protobuf messages
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
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

#endif
