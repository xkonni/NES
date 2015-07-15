/*
 * motor-daemon.cpp
 *
 * daemon to control the stepper motor controller
 * via a socket, using protobuf messages
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */

#include "motor-daemon.h"

/*
 * handle motorcommand
 * return motorstatus
 */
void handle_motorcommand (messages::motorcommand *command, messages::motorstatus *status) {
  int m;
  int steps;
  int acc;

  if (command->type() == messages::motorcommand::LOOP) {
    m = command->motor();
    steps = command->steps();
    acc = command->acc();

    // select motor
    if (m == 1) {
      motor_loop(&motor1, steps, acc);
    }
    else if (m == 2) {
      motor_loop(&motor2, steps, acc);
    }
  }
  else if (command->type() == messages::motorcommand::RESET) {
    m = command->motor();
    if ( m == 1 ) {
      motor1.pos = 0;
    }
    else if ( m == 2 ) {
      motor2.pos = 0;
    }
    else {
      motor1.pos = 0;
      motor2.pos = 0;
    }
  }

  else if (command->type() == messages::motorcommand::STATUS) {
    // sending a status anyway
  }

  // create respnse
  messages::motorstatus::motorStatusMsg *motor;
  motor = status->add_motor();
  motor->set_id(1);
  motor->set_pos(motor1.pos);
  motor = status->add_motor();
  motor->set_id(2);
  motor->set_pos(motor2.pos);
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

void socket_read_motorcommand(int sockfd) {
  int new_sockfd;
  // fds to monitor
  fd_set read_fds,write_fds;
  struct timeval waitd = {10, 0};
  int sel;
  int max_fd;
  int n;
  std::vector<int> connected;
  char buffer[BUFFERSIZE];
  // store the connecting address and size
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size;

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
      print_error("select error");
      continue;
    }

    // data received
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
        connected.push_back(new_sockfd);
      }
      for (std::vector<int>::iterator it = connected.begin(); it != connected.end(); it++) {
        if (FD_ISSET(*it, &read_fds)) {
          // skip this fd
          if (*it == sockfd) continue;
          bzero(buffer, BUFFERSIZE);
          n = read(*it, buffer, sizeof(buffer));
          // data available
          if (n > 0) {
            // message, response
            messages::motorcommand *message = new messages::motorcommand();
            messages::motorstatus *response = new messages::motorstatus();

            // parse message
            message->ParseFromString(buffer);
            print_motorcommand(message);
            // generate response
            handle_motorcommand(message, response);
            print_motorstatus(response);
            socket_write_motorstatus(*it, response);
          }
          // client disconnected
          else if (n == 0) {
            // printf("closing socket: %d\n", *it);
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

/*
 * write reply to socket
 */
void socket_write_motorstatus (int sockfd, messages::motorstatus *status) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  status->SerializeToArray(buffer, status->ByteSize());
  // send status
  write(sockfd, buffer, status->ByteSize());
}


int main(int argc, char *argv[])
{
  int sockfd;

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
  sockfd = socket_open(MOTOR_PORT);
  // main loop
  socket_read_motorcommand(sockfd);

  // exit
  printf("shutting down\n");
  shutdown(sockfd, 0);
  close(sockfd);
  return 0;
}
