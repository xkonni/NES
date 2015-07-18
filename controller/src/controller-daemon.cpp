/**
  * @file controller-daemon.cpp
  * @brief control motor- and sensor-daemon
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "controller-daemon.h"

Controller::Controller() {
  sockfd = socket_open(CONTROLLER_PORT);
}

Controller::~Controller() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

int Controller::deg2steps(int deg) {
  // TODO: add global parameter
  int total_steps = 800;
  return floor(total_steps/360*deg);
}

void Controller::calculate_movement (
    messages::sensordata *data1, messages::sensordata *data2,
    messages::motorcommand *command1, messages::motorcommand *command2) {
  double theta_diff, phi_diff;

  // motor1 -> theta
  theta_diff = data1->theta() - data2->theta();
  command1->set_type(messages::motorcommand::LOOP);
  command1->set_motor(1);
  command1->set_steps(deg2steps(theta_diff));
  command1->set_acc(10);


  // motor2 -> phi
  phi_diff = data1->phi() - data2->phi();
  command2->set_type(messages::motorcommand::LOOP);
  command2->set_motor(2);
  command2->set_steps(deg2steps(phi_diff));
  command2->set_acc(10);
}

int main(int argc, char *argv[])
{
  Controller ctrl;

  struct timeval tv_now, tv_last;
  // update timeout [usec]
  int update_timeout = 1000000;
  long int t_diff;
  int n;
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
  char buffer[BUFFERSIZE];

  messages::motorcommand *mcommand1 = new messages::motorcommand();
  messages::motorcommand *mcommand2 = new messages::motorcommand();
  messages::motorstatus *mstatus1 = new messages::motorstatus();
  messages::motorstatus *mstatus2 = new messages::motorstatus();
  messages::sensorcommand *scommand1 = new messages::sensorcommand();
  messages::sensorcommand *scommand2 = new messages::sensorcommand();
  messages::sensordata *sdata1 = new messages::sensordata();
  messages::sensordata *sdata2 = new messages::sensordata();

  // // calibrate sensor1
  // scommand1->set_type(messages::sensorcommand::CALIBRATE);
  // scommand1->set_sensor(SENSOR1);
  // bzero(buffer, BUFFERSIZE);
  // scommand1->SerializeToArray(buffer, scommand1->ByteSize());
  // n = socket_writeread(SENSOR1_PORT, SENSOR1_HOST, buffer, scommand1->ByteSize());
  // if (n > 0) {
  //   sdata1->ParseFromArray(buffer, n);
  // }
  //
  // // calibrate sensor2
  // scommand2->set_type(messages::sensorcommand::CALIBRATE);
  // scommand2->set_sensor(SENSOR2);
  // bzero(buffer, BUFFERSIZE);
  // scommand2->SerializeToArray(buffer, scommand2->ByteSize());
  // n = socket_writeread(SENSOR2_PORT, SENSOR2_HOST, buffer, scommand2->ByteSize());
  // if (n > 0) {
  //   sdata2->ParseFromArray(buffer, n);
  // }

  // initialize time
  gettimeofday(&tv_last, NULL);
  while (1) {
    // update time
    gettimeofday(&tv_now, NULL);
    t_diff = (tv_now.tv_usec - tv_last.tv_usec) + (tv_now.tv_sec - tv_last.tv_sec) * 1000000;

    // listen on socket
    n = socket_listen(ctrl.sockfd, connected, buffer);
    if (n > 0) {
      // DEBUG
      // printf("%d bytes of sensordata received!\n", n);
      // message
      messages::sensordata *message = new messages::sensordata();
      // parse message
      message->ParseFromArray(buffer, n);
      print_sensordata(NET_IN, message);
      if (message->sensor() == SENSOR1) {
        sdata1->CopyFrom(*message);
      } else {
        sdata2->CopyFrom(*message);
      }
    }

    // time to update the motors
    if (t_diff > update_timeout) {
      // DEBUG
      // printf("updating motors\n");
      mcommand1 = new messages::motorcommand();
      mcommand2 = new messages::motorcommand();

      ctrl.calculate_movement(sdata1, sdata2, mcommand1, mcommand2);
      // handle first command
      bzero(buffer, BUFFERSIZE);
      mcommand1->SerializeToArray(buffer, mcommand1->ByteSize());
      // TODO: fix
      // n = socket_writeread(MOTOR_PORT, MOTOR_HOST, buffer, mcommand1->ByteSize());
      n = socket_write(MOTOR_PORT, MOTOR_HOST, buffer, mcommand1->ByteSize());
      // success
      if (n > 0) {
        print_motorcommand(NET_OUT, mcommand1);
        mstatus1->ParseFromArray(buffer, n);
      }
      // handle second command
      bzero(buffer, BUFFERSIZE);
      mcommand2->SerializeToArray(buffer, mcommand2->ByteSize());
      // TODO: fix
      // n = socket_writeread(MOTOR_PORT, MOTOR_HOST, buffer, mcommand2->ByteSize());
      n = socket_write(MOTOR_PORT, MOTOR_HOST, buffer, mcommand2->ByteSize());
      // success
      if (n > 0) {
        print_motorcommand(NET_OUT, mcommand2);
        mstatus2->ParseFromArray(buffer, n);
      }

      // update time of last command
      gettimeofday(&tv_last, NULL);
    }

  } // while (1)

  return 0;
}
