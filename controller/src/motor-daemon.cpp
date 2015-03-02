/**
  * @file motor-daemon.cpp
  * @brief receive protobuf messages containing motorcommands, use them to control
  *   two step motors
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "motor-daemon.h"

Motor::Motor() :
    //      hdr, step, dir, pos, steps, minpos, maxpos
    motor1 {  9,   11,  12,   0,     0,   -200,    200 },
    motor2 {  9,   13,  14,   0,     0,   -200,    200 }
{
  // initialize socket
#ifdef BBB_CAN
  sockfd = can_open();
#else
  sockfd = socket_open(MOTOR_PORT);
#endif
}

Motor::~Motor() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

void Motor::handle_motorcommand (messages::motorcommand *command, messages::motorstatus *status) {
  int m = command->motor();
  int steps;

  /*
   * LOOP
   */
  if (command->type() == messages::motorcommand::LOOP) {
    steps = command->steps() - STEPS_TOTAL;
    // add [STEPS_MIN; STEPS_MAX] steps to current counter
    if (steps > STEPS_MAX) steps = STEPS_MAX;
    if (steps < STEPS_MIN) steps = STEPS_MIN;

    if (m == 1) {
      motor1.steps = steps;
      // make sure we dont exceed the limits
      if (motor1.pos + motor1.steps > motor1.maxpos)
        motor1.steps = motor1.maxpos - motor1.pos;
      if (motor1.pos + motor1.steps < motor1.minpos)
        motor1.steps = motor1.minpos - motor1.pos;
      printf("motor1 steps to go: %d\n", motor1.steps);
    }
    else if (m == 2) {
      motor2.steps = steps;
      // make sure we dont exceed the limits
      if (motor2.pos + motor2.steps > motor2.maxpos)
        motor2.steps = motor2.maxpos - motor2.pos;
      if (motor2.pos + motor2.steps < motor2.minpos)
        motor2.steps = motor2.minpos - motor2.pos;
      printf("motor2 steps to go: %d\n", motor2.steps);
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
#ifdef BBB_HOST
  pin_high(m->header, m->step);
  usleep(GPIO_HOLD);
  pin_low(m->header, m->step);
  usleep(timeout);
#endif
}

void Motor::motor_dir(motor *m, int dir) {
#ifdef BBB_HOST
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

int main(int argc, char *argv[]) {
  Motor mtr;
  int n;
  char buffer[BUFFERSIZE];
#ifndef BBB_CAN
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
#endif
  messages::motorcommand *message = new messages::motorcommand();
  messages::motorstatus *response = new messages::motorstatus();

  // initialize GPIOs
#ifdef BBB_HOST
  iolib_init();
  iolib_setdir(mtr.motor1.header, mtr.motor1.step, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor1.header, mtr.motor1.dir, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor2.header, mtr.motor2.step, BBBIO_DIR_OUT);
  iolib_setdir(mtr.motor2.header, mtr.motor2.dir, BBBIO_DIR_OUT);
#endif

  // main loop
  while (1) {
    // listen on socket
#ifdef BBB_CAN
    n = can_listen(mtr.sockfd, CAN_MOTORCOMMAND, buffer);
#else
    n = socket_listen(mtr.sockfd, connected, buffer);
#endif
    if (n > 0) {
      // parse message
      message->ParseFromArray(buffer, n);
      print_motorcommand(NET_IN, message);
      mtr.handle_motorcommand(message, response);
    }

    // do a step
    if (mtr.motor1.steps > 0) {
      mtr.motor_dir(&mtr.motor1, 0);
      mtr.motor_step(&mtr.motor1, GPIO_TIMEOUT);
      mtr.motor1.steps--;
      usleep(10000);
    }
    else if (mtr.motor1.steps < 0) {
      mtr.motor_dir(&mtr.motor1, 1);
      mtr.motor_step(&mtr.motor1, GPIO_TIMEOUT);
      mtr.motor1.steps++;
      usleep(10000);
    }
    if (mtr.motor2.steps > 0) {
      mtr.motor_dir(&mtr.motor2, 0);
      mtr.motor_step(&mtr.motor2, GPIO_TIMEOUT);
      mtr.motor2.steps--;
      usleep(10000);
    }
    else if (mtr.motor2.steps < 0) {
      mtr.motor_dir(&mtr.motor2, 1);
      mtr.motor_step(&mtr.motor2, GPIO_TIMEOUT);
      mtr.motor2.steps++;
      usleep(10000);
    }
  }

  return 0;
}
