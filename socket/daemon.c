/*
 * daemon.c
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */
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
void motor_loop (motor *m, int steps) {
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
}

/*
 * write reply to socket
 */
void socket_write (int client_sockfd, char *msg) {
  int n;
  n = write(client_sockfd, msg, BUFFERSIZE);
  if (n < 0) {
    error("ERROR writing to socket");
  }
}

int socket_read (int sockfd) {
  int n;
	int m;
  int steps;
	int client_sockfd;
  struct sockaddr_in cli_addr;
  char *substr;
  char buffer[BUFFERSIZE];
  char msg[BUFFERSIZE];
  socklen_t clilen;

  bzero(buffer, BUFFERSIZE);
  bzero(msg, BUFFERSIZE);

  clilen = sizeof(cli_addr);
  client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (client_sockfd < 0) {
    error("ERROR on accept");
    exit(1);
  }

  n = read(client_sockfd, buffer, BUFFERSIZE);
  if (n < 0) {
    error("ERROR reading from socket");
    exit(1);
  }

  /*
	 * received a command
	 */
  if (n > 0) {
    /*
     * quit
     */
    if (!strncmp (buffer, "quit", 4)) {
      printf("quit\n");
      return(0);
    }

    /*
     * turn MOTOR STEPS
     */
    else if (!strncmp (buffer, "loop", 4)) {
      substr = strtok(buffer, " ");
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
      socket_write(client_sockfd, msg);

      return(1);
    }

    else if (!strncmp (buffer, "reset", 5)) {
      substr = strtok(buffer, " ");
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
      socket_write(client_sockfd, msg);
      return(1);
		}

    /*
     * status
     */
    else if (!strncmp (buffer, "status", 6)) {
      sprintf(msg, "STATUS position: %d/%d", motor1.pos, motor2.pos);
      socket_write(client_sockfd, msg);
      printf("%s\n", msg);
      return(1);
    }

    /*
     * all else
     */
    else
      sprintf(msg, "received unknown command: %s\n", buffer);
      socket_write(client_sockfd, msg);
      printf("%s\n", msg);
      return(1);
  }

	shutdown(client_sockfd, 0);
	close(client_sockfd);
  return(1);
}

int main(int argc, char *argv[])
{
  // define variables
  int sockfd;
  struct sockaddr_in serv_addr;

  // initialize GPIOs
  iolib_init();
  iolib_setdir(motor1.header, motor1.step, BBBIO_DIR_OUT);
  iolib_setdir(motor1.header, motor1.dir, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.step, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.dir, BBBIO_DIR_OUT);

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

  // main loop
  while (socket_read(sockfd));

	printf("shutting down\n");
  shutdown(sockfd, 0);
  close(sockfd);
  return 0;
}
