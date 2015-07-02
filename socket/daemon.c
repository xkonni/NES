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
void motor_step(motor *m) {
	pin_high(m->header, m->step);
	usleep(GPIO_HOLD);
	pin_low(m->header, m->step);
	usleep(GPIO_TIMEOUT);
}

/*
 * change direction
 */
void motor_dir(motor *m, int dir) {
  if (dir == 0) {
		if (is_high(m->header, m->dir))
			pin_low(m->header, m->dir);
  }
  else {
		if (is_low(m->header, m->dir))
			pin_high(m->header, m->dir);
  }
}

/*
 * do n steps
 */
int motor_loop (motor *m, int steps) {
  if (steps > 0) {
    steps = m->pos + steps <= MAX_POS ? steps : MAX_POS - m->pos;
    motor_dir(m, 0);
  }
  else {
    steps = m->pos + steps >= MIN_POS ? steps : MIN_POS - m->pos;
    motor_dir(m, 1);
  }

	int n;
	for (n = 0; n < abs(steps); n++) {
		motor_step(m);
  }
  m->pos += steps;

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
	int m;
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
     * turn MOTOR STEPS
     */
    else if (!strncmp (buf, "loop", 4)) {
      substr = strtok(buf, " ");
      substr = strtok (NULL, " ");
      m = atoi(substr);
      substr = strtok (NULL, " ");
      steps = atoi(substr);

			// select motor
			if (m == 1) {
				motor_loop(&motor1, steps);
			}
			else if (m == 2) {
				motor_loop(&motor2, steps);
			}

      sprintf(msg, "LOOP motor: %d steps: %d", m, steps);
      printf("%s\n", msg);
      socket_write(sock, msg);

      return(0);
    }

    else if (!strncmp (buf, "reset", 5)) {
      substr = strtok(buf, " ");
      substr = strtok (NULL, " ");
      m = atoi(substr);

			// select motor
			if (m == 0) {
				motor1.pos = 0;
				motor2.pos = 0;
				sprintf(msg, "RESET motor1 and motor2");
			}
			else if (m == 1) {
				motor1.pos = 0;
				sprintf(msg, "RESET motor1");
			}
			else if (m == 2) {
				motor2.pos = 0;
				sprintf(msg, "RESET motor2");
			}

      printf("%s\n", msg);
      socket_write(sock, msg);
		}

    /*
     * status
     */
    else if (!strncmp (buf, "status", 6)) {
      sprintf(msg, "STATUS position: %d/%d", motor1.pos, motor2.pos);
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
		shutdown(client_sockfd, 0);
		close(client_sockfd);
  }
  shutdown(sockfd, 0);
  close(sockfd);
  return 0;
}
