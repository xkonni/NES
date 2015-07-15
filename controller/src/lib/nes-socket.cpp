/*
 * nes-socket.cpp
 *
 * functions for sockets
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */
#include "nes-socket.h"
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

/*
 * set socket to non-blocking
 */
void socket_setnonblock(int sockfd) {
    int flags;
    flags = fcntl(sockfd,F_GETFL,0);
    assert(flags != -1);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}
