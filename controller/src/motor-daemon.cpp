/**
  * @file motor-daemon.cpp
  * @brief receive protobuf messages containing motorcommands, use them to control 
  *   two step motors
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "motor-daemon.h"

Motor::Motor() :
    ramp {100,  71,  53,  42,  33,  27,  23,  19,  16,  14,
           12,  11,  10,   9,   8,   7,   6,   6,   5,   5,
            5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
            2,   2,   2,   2,   2,   2,   2,   2,   2,   2 },
    motor1 { 9, 11, 12, 10, 0, -200, 200 },
    motor2 { 9, 13, 14, 10, 0, -200, 200 }
{
  // initialize socket
  sockfd = socket_open(MOTOR_PORT);
}

Motor::~Motor() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

void Motor::handle_motorcommand (messages::motorcommand *command, messages::motorstatus *status) {
  int m = command->motor();

  /*
   * LOOP
   */
  if (command->type() == messages::motorcommand::LOOP) {
    if (m == 1) {
      motor_loop(&motor1, command->steps());
    }
    else if (m == 2) {
      motor_loop(&motor2, command->steps());
    }
  }

  /*
   * ACC
   */
  else if (command->type() == messages::motorcommand::ACC) {
    int acc = command->acc();
    if (acc < 1) {
      printf("acc too low, setting from %d to 1\n", acc);
      acc = 1;
    }
    else if (acc > 10) {
      printf("acc too high, setting from %d to 10\n", acc);
      acc = 10;
    }

    if ( m == 1 ) {
      motor1.acc = acc;
    }
    else if ( m == 2 ) {
      motor2.acc = acc;
    }
  }

  /*
   * RESET
   */
  else if (command->type() == messages::motorcommand::RESET) {
    if ( m == 1 ) {
      motor1.pos = 0;
    }
    else if ( m == 2 ) {
      motor2.pos = 0;
    }
  }

  /*
   * STATUS
   */
  else if (command->type() == messages::motorcommand::STATUS) {
    status->set_motor(m);
    if ( m == 1 ) {
      status->set_pos(motor1.pos);
    }
    else if ( m == 2 ) {
      status->set_pos(motor2.pos);
    }
  }
}

void Motor::motor_step(motor *m, int timeout) {
#ifdef HOST_BBB
  pin_high(m->header, m->step);
  usleep(GPIO_HOLD);
  pin_low(m->header, m->step);
  usleep(timeout);
#endif
}

void Motor::motor_dir(motor *m, int dir) {
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

void Motor::motor_loop (motor *m, int steps) {
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
      delay = (10*ramp[n])/m->acc;
      // printf("delay+: %f\n", GPIO_TIMEOUT * delay);
    }
    // deceleration
    else if ( abs(steps) - n < rampN) {
      delay = (10*ramp[abs(steps)-n])/m->acc;
      // printf("delay-: %f\n", GPIO_TIMEOUT * delay);
    }
    // run
    else delay = 1;
    motor_step(m, GPIO_TIMEOUT * delay);
  }
  m->pos += steps;
}

int main(int argc, char *argv[]) {
  Motor mtr;
  int n;
  char buffer[BUFFERSIZE];
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
  messages::motorcommand *message = new messages::motorcommand();
  messages::motorstatus *response = new messages::motorstatus();

  // initialize GPIOs
#ifdef HOST_BBB
  iolib_init();
  iolib_setdir(mtr.motor1.header, mtr.motor1.step, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor1.header, mtr.motor1.dir, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor2.header, mtr.motor2.step, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor2.header, mtr.motor2.dir, BBBIO_DIR_OUT);
#endif

  // main loop
  while (1) {
    // listen on socket
    n = socket_listen(mtr.sockfd, connected, buffer);
    if (n > 0) {
      // parse message
      message->ParseFromArray(buffer, n);
      print_motorcommand(NET_IN, message);
      mtr.handle_motorcommand(message, response);
    }
    // TODO: fix
    // command requested response
    // if (response->motor_size() > 0) {
    //   bzero(buffer, BUFFERSIZE);
    //   response->SerializeToArray(buffer, response->ByteSize());
    //   socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, response->ByteSize());
    // }
  }

  return 0;
}
