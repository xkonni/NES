/*
 * daemon.c
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "daemon.h"

int status_pos, status_dir;

/*
 * print error and exit
 */
void error(const char *reply) {
  perror(reply);
  exit(1);
}

/*
 * do a single step
 */
void motor_step(motor m) {
	pin_high(m.header, m.step);
	usleep(GPIO_HOLD);
	pin_low(m.header, m.step);
	usleep(GPIO_TIMEOUT);
}

/*
 * change direction
 */
void motor_dir(motor m, int dir) {
	if (dir == 0) {
	  pin_low(m.header, m.dir);
	  status_dir = 0;
	}
	else {
	  pin_high(m.header, m.dir);
	  status_dir = 1;
	}
}

/*
 * do n steps
 */
int motor_loop (motor m, int steps) {
  if (steps > 0) {
    steps = status_pos + steps <= MAX_POS ? steps : MAX_POS - status_pos;
    motor_dir(m, 0);
  }
  else {
    steps = status_pos + steps >= MIN_POS ? steps : MIN_POS - status_pos;
    motor_dir(m, 1);
  }

	int n;
	for (n = 0; n < abs(steps); n++) {
		motor_step(m);
  }
  status_pos += steps;

  return(0);
}

/*
 * write reply to socket
 */
int socket_write (int sock, char *buf) {
  int n;
  n = write(sock, buf, BUFFERSIZE);
  if (n < 0) {
    error("ERROR writing to socket");
  }
  return(0);
}

int socket_read (int sock, char *buf) {
  int n;
  int steps;
  char *substr;
  char msg[BUFFERSIZE];

  bzero(buf, BUFFERSIZE);
  bzero(msg, BUFFERSIZE);
  n = read(sock, buf, BUFFERSIZE);
  if (n < 0) {
    error("ERROR reading from socket");
    exit(1);
  }
  if (n > 0) {

    /*
     * quit
     */
    if (!strncmp (buf, "quit", 4)) {
      printf("quit\n");
      return(1);
    }

    /*
     * turn DIR STEPS
     */
    else if (!strncmp (buf, "turn", 4)) {
      substr = strtok(buf, " ");
      substr = strtok (NULL, " ");
      steps = atoi(substr);

      sprintf(msg, "TURN steps: %d", steps);
      printf("%s\n", msg);
      socket_write(sock, msg);
      motor_loop(motor1, steps);
      return(0);
    }

    /*
     * status
     */
    else if (!strncmp (buf, "status", 6)) {
      sprintf(msg, "STATUS position: %d", status_pos);
      socket_write(sock, msg);
      printf("%s\n", msg);
      return(0);
    }

    /*
     * all else
     */
    else
      printf("received unknown command: %s\n", buf);
      // char reply[] = "i got your message";
      // socket_write(sock, reply);
      return(0);
  }

  return(0);
}

int main(int argc, char *argv[])
{
  // define variables
  int sockfd, client_sockfd;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int ret;

  // initialize GPIOs
  iolib_init();
  iolib_setdir(motor1.header, motor1.step, BBBIO_DIR_OUT);
  iolib_setdir(motor1.header, motor1.dir, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.step, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.dir, BBBIO_DIR_OUT);

  // initialize status
  ret = 0;

  // initialize socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // avoid "this address is already in use"
  int so_reuseaddr = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

  if (sockfd < 0)
    error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd,5);

  clilen = sizeof(cli_addr);
  while (ret == 0) {
    client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (client_sockfd < 0) {
      error("ERROR on accept");
      exit(1);
    }

    ret = socket_read(client_sockfd, buffer);
  }
  shutdown(client_sockfd, 0);
  shutdown(sockfd, 0);
  close(client_sockfd);
  close(sockfd);
  return 0;
}
