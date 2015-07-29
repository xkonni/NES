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


int can_listen(int sockfd, int canid, char *buffer) {
  int max_fd;
  int sel;
  int n;
  struct can_frame frame;
  // timeout {[sec], [usec]}
  struct timeval waitd = {0, 100};
  // fds to monitor
  fd_set read_fds, write_fds;

  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  // add sockfd
  FD_SET(sockfd, &read_fds);
  FD_SET(sockfd, &write_fds);
  max_fd = sockfd;

  // check for data
  sel = select(max_fd+1, &read_fds, &write_fds, (fd_set*)0, &waitd);

  // continue on error
  if (sel < 0) {
    perror("select error");
    return(0);
  }

  // socket active
  if (sel > 0) {
    if(FD_ISSET(sockfd, &read_fds)) {
      n = read(sockfd, &frame, sizeof(frame));
      if (frame.can_id & canid) {
        strncpy(buffer, (char *)frame.data, frame.can_dlc);
        // attach can_id to n [0..255 | 256 .. 2048]
        n = n | (frame.can_id << 8);
        // return sent size
        return(n);
      }
      else {
        printf("ignored canid: %d\n", frame.can_id);
      }
    }
  } // if (sel > 0)
  return(0);
}

int can_write(int sockfd, int canid, const char *buffer, int size) {
  int n;
  int i;
  struct can_frame frame;

  // fill can_frame
  frame.can_id = canid;
  for (i = 0; i < size; i++)
    frame.data[i] = buffer[i];
  frame.can_dlc = size;

  // Send a message to the CAN bus
  n = write(sockfd, &frame, sizeof(frame));

  // return sent size
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

  // return fd
  return sockfd;
}
