/*
 * motor-daemon.cpp
 *
 * daemon to control the stepper motor controller
 * via a socket
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */
#include "motor-daemon.h"

motor motor1, motor2;

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
void motor_step(motor *m, int timeout) {
#ifdef HOST_BBB
  pin_high(m->header, m->step);
  usleep(GPIO_HOLD);
  pin_low(m->header, m->step);
  usleep(timeout);
#endif
}

/*
 * change direction
 */
void motor_dir(motor *m, int dir) {
#ifdef HOST_BBB
  if (dir == 0) {
    if (is_high(m->header, m->dir))
      pin_low(m->header, m->dir);
  }
  else {
    if (is_low(m->header, m->dir))
      pin_high(m->header, m->dir);
  }
#endif
}

/*
 * do n steps
 *
 * motor *m   motor to use
 * int steps  number of steps
 * int acc    acceleration
 *            [1 (slow) .. 10 (fast) ]
 */
void motor_loop (motor *m, int steps, int acc) {
  if (acc < 1) {
    printf("acc too low, setting from %d to 1\n", acc);
    acc = 1;
  }
  else if (acc > 10) {
    printf("acc too high, setting from %d to 10\n", acc);
    acc = 10;
  }

  if (steps > 0) {
    steps = m->pos + steps <= m->maxpos ? steps : m->maxpos - m->pos;
    motor_dir(m, 0);
  }
  else {
    steps = m->pos + steps >= m->minpos ? steps : m->minpos - m->pos;
    motor_dir(m, 1);
  }

  int n;
  float delay;
  for (n = 0; n < abs(steps); n++) {
    // acceleration
    if ( n < rampN ) {
      delay = (10*ramp[n])/acc;
      // printf("delay+: %f\n", GPIO_TIMEOUT * delay);
    }
    // deceleration
    else if ( abs(steps) - n < rampN) {
      delay = (10*ramp[abs(steps)-n])/acc;
      // printf("delay-: %f\n", GPIO_TIMEOUT * delay);
    }
    // run
    else delay = 1;
    motor_step(m, GPIO_TIMEOUT * delay);
  }
  m->pos += steps;
}

/*
 * write reply to socket
 */
void socket_write (int client_sockfd, messages::motorstatus *response) {
  // int n;
  // n = write(client_sockfd, msg, BUFFERSIZE);
  if (! response->SerializeToFileDescriptor(client_sockfd) ) {
    error("ERROR writing to socket");
  }
  printf("written\n");
}

/*
 * read from socket
 */
int socket_read (int sockfd) {
  // int n;
  int m;
  int steps;
  int acc;
  char msg[BUFFERSIZE];
  bzero(msg, BUFFERSIZE);

  int client_sockfd;
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  clilen = sizeof(cli_addr);
  client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (client_sockfd < 0) {
    error("ERROR on accept");
    exit(1);
  }

  // read motorcommand
  messages::motorcommand *message = new messages::motorcommand();
  message->ParseFromFileDescriptor(client_sockfd);
  // printf("motorcommand %d\n", message->type());

  if (message->type() == messages::motorcommand::LOOP) {
    m = message->motor();
    steps = message->steps();
    acc = message->acc();
    sprintf(msg, "LOOP motor: %d steps: %d acc: %d", m, steps, acc);

    // select motor
    if (m == 1) {
      motor_loop(&motor1, steps, acc);
    }
    else if (m == 2) {
      motor_loop(&motor2, steps, acc);
    }
    else {
      printf("magic!\n");
    }
    printf("%s\n", msg);
    return(1);
  }
  else if (message->type() == messages::motorcommand::RESET) {
    m = message->motor();
    if ( m == 1 ) {
      motor1.pos = 0;
      sprintf(msg, "RESET motor1");
    }
    else if ( m == 2 ) {
      motor2.pos = 0;
      sprintf(msg, "RESET motor2");
    }
    else {
      motor1.pos = 0;
      motor2.pos = 0;
      sprintf(msg, "RESET motor1 and motor2");
    }
    printf("%s\n", msg);
    return(1);
  }

  else if (message->type() == messages::motorcommand::STATUS) {
    m = message->motor();
    if ( m == 1 ) {
      sprintf(msg, "motor1 pos: %d", motor1.pos);
    }
    else if ( m == 2 ) {
      sprintf(msg, "motor2 pos: %d", motor2.pos);
    }
    else {
      sprintf(msg, "motor1 pos: %d, motor2 pos: %d", motor1.pos, motor2.pos);
    }
    printf("%s\n", msg);
    return(1);
  }

  // write motorstatus
  // messages::motorstatus *response = new messages::motorstatus();
  // messages::motorstatus::motorStatusMsg *motor;
  // motor = response->add_motor();
  // motor->set_id(1);
  // motor->set_pos(motor1.pos);
  // motor = response->add_motor();
  // motor->set_id(1);
  // motor->set_pos(motor2.pos);
  // printf("writing response...\n");
  // socket_write(sockfd, response);
  // printf("... done\n");

  shutdown(client_sockfd, 0);
  close(client_sockfd);
  return(1);
}

int main(int argc, char *argv[])
{
  // define variables
  int sockfd;
  struct sockaddr_in serv_addr;

  // initialize motors
  motor1 = (motor) { 8, 11, 12, 0, -400, 400 };
  motor2 = (motor) { 8, 13, 14, 0, -200, 200 };

  // initialize GPIOs
#ifdef HOST_BBB
  iolib_init();
  iolib_setdir(motor1.header, motor1.step, BBBIO_DIR_OUT);
  iolib_setdir(motor1.header, motor1.dir, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.step, BBBIO_DIR_OUT);
  iolib_setdir(motor2.header, motor2.dir, BBBIO_DIR_OUT);
#endif

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
