/**
  * @file controller-daemon.cpp
  * @brief control motor- and sensor-daemon
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "controller-daemon.h"

Controller::Controller() {
#ifdef BBB_CAN
  sockfd = can_open();
#else
  sockfd = socket_open(CONTROLLER_PORT);
#endif
}

Controller::~Controller() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

int Controller::deg2steps(int deg) {
  int steps;

  steps = floor(STEPS_TOTAL / 360 * deg);
  if (abs(steps) > 40) { }
  if (abs(steps) > 20) {
    steps = floor(steps * 0.8);
  }
  else if (abs(steps) > 10) {
    steps = floor(steps * 0.7);
  }
  else if (abs(steps) > 5) {
    steps = 2;
  }
  else if (abs(steps) > 3) {
    steps = 1;
  }
  else {
    steps = 0;
  }

  // if ((steps < BLINDSPOT) && (steps > -BLINDSPOT)) {
  //   steps = 0;
  // }

  // dont move too much
  return steps;
}

void Controller::calculate_movement (
    messages::sensordata *data1, messages::sensordata *data2,
    messages::motorcommand *command1, messages::motorcommand *command2) {
  int steps;

  // motor1 -> theta
  command1->set_type(messages::motorcommand::LOOP);
  command1->set_motor(1);
  steps = deg2steps(data1->theta() - data2->theta());
  // modify steps [-800; 800] -> [0; 1600]
  command1->set_steps(steps + 800);


  // motor2 -> phi
  command2->set_type(messages::motorcommand::LOOP);
  command2->set_motor(2);
  steps = deg2steps(data1->phi() - data2->phi());
  // modify steps [-800; 800] -> [0; 1600]
  command2->set_steps(steps + 800);
}

int Controller::send_motorcommand(messages::motorcommand *command) {
  char buffer[BUFFERSIZE];
  int n;

  command->SerializeToArray(buffer, command->ByteSize());
#ifdef BBB_CAN
  n = can_write(sockfd, CAN_MOTORCOMMAND, buffer, command->ByteSize());
#else
  n = socket_write(MOTOR_PORT, MOTOR_HOST, buffer, command->ByteSize());
#endif
  if (n > 0) {
    print_motorcommand(NET_OUT, command);
  }
  return(n);
}

int Controller::send_sensorcommand(messages::sensorcommand *command) {
  char buffer[BUFFERSIZE];
  int n;

  command->SerializeToArray(buffer, command->ByteSize());
#ifdef BBB_CAN
    n = can_write(sockfd, CAN_SENSORCOMMAND, buffer, command->ByteSize());
#else
  if (command->sensor() == SENSOR1)
    n = socket_write(SENSOR1_PORT, SENSOR1_HOST, buffer, command->ByteSize());
  else
    n = socket_write(SENSOR2_PORT, SENSOR2_HOST, buffer, command->ByteSize());
#endif
  if (n > 0) {
    print_sensorcommand(NET_OUT, command);
  }
  return(n);
}

int main(int argc, char *argv[])
{
  Controller ctrl;

  messages::sensordata *sdata1 = new messages::sensordata();
  messages::sensordata *sdata2 = new messages::sensordata();
  messages::motorcommand *mcommand1 = new messages::motorcommand();
  messages::motorcommand *mcommand2 = new messages::motorcommand();

  struct timeval tv_now, tv_last;
  long int t_diff;
  int n;
  char buffer[BUFFERSIZE];
#ifndef BBB_CAN
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
#endif
  // message id
  int msg_id;

  // initialize time
  gettimeofday(&tv_last, NULL);
  while (1) {
    // listen on socket
#ifdef BBB_CAN
    n = can_listen(ctrl.sockfd, CAN_SENSORDATA | CAN_MOTORSTATUS, buffer);
#else
    n = socket_listen(ctrl.sockfd, connected, buffer);
#endif
    if (n > 0) {
#ifdef BBB_CAN
      // extract can id
      msg_id = n >> 8;
#else
      // set to sensordata
      msg_id = CAN_SENSORDATA;
#endif
      // and actual size
      n = n & 0xff;
      // motorstatus
      messages::motorstatus *status = new messages::motorstatus();
      // sensordata
      messages::sensordata *data = new messages::sensordata();

      // sensordata
      if (msg_id == CAN_SENSORDATA) {
        data->ParsePartialFromArray(buffer, n);
        print_sensordata(NET_IN, data);
        if (data->sensor() == SENSOR1) {
          sdata1->CopyFrom(*data);
        } else {
          sdata2->CopyFrom(*data);
        }
      }
      // motorstatus
      else if (msg_id == CAN_MOTORSTATUS) {
        status->ParsePartialFromArray(buffer, n);
        print_motorstatus(NET_IN, status);
      }
    }

    // update time
    gettimeofday(&tv_now, NULL);
    t_diff = (tv_now.tv_usec - tv_last.tv_usec) + (tv_now.tv_sec - tv_last.tv_sec) * 1000000;

    // time to update the motors
    if (t_diff > TIMEOUT_MOTOR) {
      // DEBUG
      // printf("updating motors\n");
      mcommand1->Clear();
      mcommand2->Clear();

      ctrl.calculate_movement(sdata1, sdata2, mcommand1, mcommand2);
      ctrl.send_motorcommand(mcommand1);
      ctrl.send_motorcommand(mcommand2);

      // update time of last command
      gettimeofday(&tv_last, NULL);
    }

  } // while (1)

  return 0;
}
