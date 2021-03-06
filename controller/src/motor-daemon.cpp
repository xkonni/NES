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
    motor1 {  9,   11,  12,   0,     0,   -150,    150 },
    motor2 {  9,   13,  14,   0,     0,   -150,    150 }
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
      // make sure we dont exceed the limits
      if (motor1.pos + motor1.steps > motor1.maxpos) {
        steps = motor1.maxpos - motor1.pos;
        // hit the edge, send back status
        command->set_type(messages::motorcommand::STATUS);
      }
      if (motor1.pos + motor1.steps < motor1.minpos) {
        steps = motor1.minpos - motor1.pos;
        // hit the edge, send back status
        command->set_type(messages::motorcommand::STATUS);
      }
      motor1.steps = steps;
      // printf("motor1 pos: %d/[%d, %d], steps to go: %d\n",
      //     motor1.pos, motor1.minpos, motor1.maxpos, motor1.steps);
    }
    else if (m == 2) {
      motor2.steps = steps;
      // make sure we dont exceed the limits
      if (motor2.pos + motor2.steps > motor2.maxpos) {
        steps = motor2.maxpos - motor2.pos;
        // hit the edge, send back status
        command->set_type(messages::motorcommand::STATUS);
      }
      if (motor2.pos + motor2.steps < motor2.minpos) {
        steps = motor2.minpos - motor2.pos;
        // hit the edge, send back status
        command->set_type(messages::motorcommand::STATUS);
      }
      motor2.steps = steps;
      // printf("motor2 pos: %d/[%d, %d], steps to go: %d\n",
      //     motor2.pos, motor2.minpos, motor2.maxpos, motor2.steps);
    }
  }

  /*
   * STATUS
   * ... or hit the edge
   */
  if (command->type() == messages::motorcommand::STATUS) {
    status->set_motor(m);
    if ( m == 1 ) {
      status->set_pos(motor1.pos + 800);
    }
    else if ( m == 2 ) {
      status->set_pos(motor2.pos + 800);
    }
    send_motorstatus(status);
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

int Motor::send_motorstatus(messages::motorstatus *status) {
   char buffer[BUFFERSIZE];
   int n;

   status->SerializePartialToArray(buffer, status->ByteSize());
#ifdef BBB_CAN
   n = can_write(sockfd, CAN_MOTORSTATUS, buffer, status->ByteSize());
#else
   n = socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, status->ByteSize());
#endif
   if (n > 0) {
     print_motorstatus(NET_OUT, status);
   }
   return(n);
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
      message->ParsePartialFromArray(buffer, n);
      print_motorcommand(NET_IN, message);
      mtr.handle_motorcommand(message, response);
    }

    int TIMEOUT_STEP = 5000;
    // do a step
    if (mtr.motor1.steps > 0) {
      mtr.motor_dir(&mtr.motor1, 0);
      mtr.motor_step(&mtr.motor1, GPIO_TIMEOUT);
      mtr.motor1.steps--;
      mtr.motor1.pos++;
      usleep(TIMEOUT_STEP);
    }
    else if (mtr.motor1.steps < 0) {
      mtr.motor_dir(&mtr.motor1, 1);
      mtr.motor_step(&mtr.motor1, GPIO_TIMEOUT);
      mtr.motor1.steps++;
      mtr.motor1.pos--;
      usleep(TIMEOUT_STEP);
    }
    if (mtr.motor2.steps > 0) {
      mtr.motor_dir(&mtr.motor2, 0);
      mtr.motor_step(&mtr.motor2, GPIO_TIMEOUT);
      mtr.motor2.steps--;
      mtr.motor2.pos++;
      usleep(TIMEOUT_STEP);
    }
    else if (mtr.motor2.steps < 0) {
      mtr.motor_dir(&mtr.motor2, 1);
      mtr.motor_step(&mtr.motor2, GPIO_TIMEOUT);
      mtr.motor2.steps++;
      mtr.motor2.pos--;
      usleep(TIMEOUT_STEP);
    }
  }

  return 0;
}
