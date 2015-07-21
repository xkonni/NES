/**
  * @file nes-can.h
  * @brief handle can sockets
  *
  * @details
  *   This file handles can sockets
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "nes-can.h"


int can_listen(int sockfd, std::vector<int> *connected, char *buffer) {
  return(0);
}

int can_write(int sockfd, const char *buffer, int size) {
  int n;

  // Send a message to the CAN bus
  struct can_frame frame;
  frame.can_id = 0x123;
  strcpy((char *)frame.data, buffer);;

  frame.can_dlc = strlen((char *)frame.data);
  n = write(sockfd, &frame, sizeof(frame));

  // // Read a message back from the CAN bus
  // int bytes_read = read(sockfd, &frame, sizeof(frame));
  return(n);
}

int can_open() {
  // Create the socket
  int sockfd = socket( PF_CAN, SOCK_RAW, CAN_RAW );

  // Locate the interface you wish to use
  struct ifreq ifr;
  strcpy(ifr.ifr_name, "can0");
  // ifr.ifr_ifindex gets filled with that device's index
  ioctl(sockfd, SIOCGIFINDEX, &ifr);

  // Select that CAN interface, and bind the socket to it
  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  bind(sockfd, (struct sockaddr*)&addr, sizeof(addr) );

  return sockfd;
}
