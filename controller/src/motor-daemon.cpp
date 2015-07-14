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

void socket_setnonblock(int socket) {
    int flags;
    flags = fcntl(socket,F_GETFL,0);
    assert(flags != -1);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

int socket_open() {
  // define variables
  int sockfd, new_sockfd;
  int max_fd;
  int n;
  int status;
  std::vector<int> connected;
  char buffer[256];
  struct addrinfo hints;
  struct addrinfo *servinfo;
  // store the connecting address and size
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size;

  // fds to monitor
  fd_set read_fds,write_fds;      // the flag sets to be used
  struct timeval waitd = {10, 0};     // the max wait time for an event
  int sel;                            // holds return value for select();


  // socket info
  memset(&hints, 0, sizeof hints);    // make sure the struct is empty
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;    // tcp
  hints.ai_flags = AI_PASSIVE;        // use local-host address

  // get server info, put into servinfo
  if ((status = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
  }

  // initialize socket
  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  if (sockfd < 0) {
      error("server socket failure");
  }

  // avoid "this address is already in use"
  int yes = 1;
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
      error("setsockopt");
  }

  //unlink and bind
  unlink("localhost");
  if(bind (sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
      error("bind error");
  }

  // gc
  freeaddrinfo(servinfo);

  if(listen(sockfd, 5) < 0) {
      error("Listen error");
  }

  while (1) {
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    // add sockfd
    FD_SET(sockfd, &read_fds);
    FD_SET(sockfd, &write_fds);
    max_fd = sockfd;
    // add connected clients
    for (std::vector<int>::iterator it = connected.begin(); it != connected.end(); it++) {
      FD_SET(*it, &read_fds);
      max_fd = std::max(max_fd, *it);
    }

    // check for data
    sel = select(max_fd+1, &read_fds, &write_fds, (fd_set*)0, &waitd);

    // continue on error
    if (sel < 0) {
      error("select error");
      continue;
    }

    // data received
    if (sel > 0) {
      if(FD_ISSET(sockfd, &read_fds)) {
        printf("data on sockfd\n");
        their_addr_size = sizeof(their_addr);
        new_sockfd = accept(sockfd, (struct sockaddr*)&their_addr, &their_addr_size);
        if( new_sockfd < 0) {
            error("accept error");
        }
        socket_setnonblock(new_sockfd);
        connected.push_back(new_sockfd);
      }
      for (std::vector<int>::iterator it = connected.begin(); it != connected.end(); it++) {
        if (FD_ISSET(*it, &read_fds)) {
          bzero(buffer, BUFFERSIZE);
          n = read(*it, buffer, sizeof(buffer));
          if (n > 0) {
            printf("data on sock[%d] %d bytes: %s", *it, n, buffer);
          }
          if (n == 0) {
            printf("closing socket: %d\n", *it);
            // shutdown, close socket
            shutdown(*it, SHUT_RDWR);
            close(*it);
            // erase from list
            connected.erase(it);
            // iterator invalid, end for-loop
            break;
          }
        }
      } // for
    } // if (sel > 0)
  } // while (1)
}

int main(int argc, char *argv[])
{
  int sockfd = socket_open();

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

  // main loop
  // while (socket_read(sockfd));
  // socket_read(sockfd);
  printf("shutting down\n");
  shutdown(sockfd, 0);
  close(sockfd);
  return 0;
}
