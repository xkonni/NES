/**
  * @file nes-socket.cpp
  * @brief handle sockets
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "nes-socket.h"

int socket_listen(int sockfd, std::vector<int> *connected, char *buffer) {
  int new_sockfd;
  int max_fd;
  int sel;
  int n;
  // timeout {[sec], [usec]}
  struct timeval waitd = {0, 100};
  // fds to monitor
  fd_set read_fds,write_fds;
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size;

  // listen to sensorstatus messages
  // printf("listening...\n");
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  // add sockfd
  FD_SET(sockfd, &read_fds);
  FD_SET(sockfd, &write_fds);
  max_fd = sockfd;

  // add connected clients
  for (std::vector<int>::iterator it = connected->begin(); it != connected->end(); it++) {
    FD_SET(*it, &read_fds);
    max_fd = std::max(max_fd, *it);
  }

  // check for data
  sel = select(max_fd+1, &read_fds, &write_fds, (fd_set*)0, &waitd);

  // continue on error
  if (sel < 0) {
    perror("select error");
    return(0);
  }

  // socket active
  if (sel > 0) {
    // client connected
    if(FD_ISSET(sockfd, &read_fds)) {
      // printf("data on sockfd\n");
      their_addr_size = sizeof(their_addr);
      new_sockfd = accept(sockfd, (struct sockaddr*)&their_addr, &their_addr_size);
      if( new_sockfd < 0) {
          print_error("accept error");
      }
      socket_setnonblock(new_sockfd);
      connected->push_back(new_sockfd);
    }
    for (std::vector<int>::iterator it = connected->begin(); it != connected->end(); it++) {
      if (FD_ISSET(*it, &read_fds)) {
        // skip this fd
        if (*it == sockfd) continue;
        bzero(buffer, BUFFERSIZE);
        n = read(*it, buffer, BUFFERSIZE);

        // data received
        if (n > 0) {
          return(n);
        }
        // client disconnected
        if (n == 0) {
          // shutdown, close socket
          shutdown(*it, SHUT_RDWR);
          close(*it);
          // erase from list
          connected->erase(it);
          // iterator invalid, end for-loop
          break;
        }
      }
    } // for
  } // if (sel > 0)
  return(0);
}

int socket_write(int port, const char *hostname, const char *buffer, int size) {
  int sockfd;
  int n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
      print_error("ERROR opening socket");

  server = gethostbyname(hostname);
  if (server == NULL) {
      print_error("ERROR, no such host");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(port);

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
      perror("ERROR connecting\n");
      usleep(500000);
      return(-1);
  }

  n = write(sockfd, buffer, size);
  if (n < 0) print_error("socket_writeread: error on writing");

  shutdown(sockfd, 0);
  close(sockfd);

  return n;
}

int socket_writeread(int port, const char *hostname, char *buffer, int size) {
  int sockfd;
  int n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
      print_error("ERROR opening socket");

  server = gethostbyname(hostname);
  if (server == NULL) {
      print_error("ERROR, no such host");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(port);

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
      perror("ERROR connecting\n");
      usleep(500000);
      return(-1);
  }

  n = write(sockfd, buffer, size);
  if (n < 0) print_error("socket_writeread: error on writing");

  bzero(buffer, BUFFERSIZE);
  n = read(sockfd, buffer, BUFFERSIZE);
  if (n < 0) print_error("socket_writeread: error on reading");

  shutdown(sockfd, 0);
  close(sockfd);

  return n;
}

int socket_open(int port) {
  int sockfd;

  // initialize socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // avoid "this address is already in use"
  int so_reuseaddr = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

  if (sockfd < 0)
    print_error("ERROR opening socket");

  struct sockaddr_in serv_addr;
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    print_error("ERROR on binding");

  // avoid "this address is already in use"
  int yes = 1;
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
    print_error("setsockopt");

  if(listen(sockfd, 5) < 0)
    print_error("Listen error");

  return(sockfd);
}

void socket_setnonblock(int sockfd) {
    int flags;
    flags = fcntl(sockfd,F_GETFL,0);
    assert(flags != -1);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}
