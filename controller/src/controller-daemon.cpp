/**
  * @file controller-daemon.cpp
  * @brief control motor- and sensor-daemon
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */
#include "controller-daemon.h"

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::socket_write_motorcommand (
    messages::motorcommand *command, messages::motorstatus *status) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;
  int n;

  client_sockfd = socket_connect(MOTOR_PORT, MOTOR_HOST);
  if (! command->SerializeToFileDescriptor(client_sockfd) ) {
    print_error("ERROR writing to socket");
  }
  print_motorcommand(NET_OUT, command);

  n = read(client_sockfd, buffer, BUFFERSIZE);
  if (n > 0) {
    status->ParseFromArray(buffer, n);
    print_motorstatus(NET_IN, status);
  }

  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
}

int deg2steps(int deg) {
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
  command1->set_motor(1);
  command1->set_steps(deg2steps(theta_diff));
  command1->set_acc(10);


  // motor2 -> phi
  phi_diff = data1->phi() - data2->phi();
  command2->set_motor(2);
  command2->set_steps(deg2steps(phi_diff));
  command2->set_acc(10);
}

void Controller::socket_write_sensorcommand (
    messages::sensorcommand *command, messages::sensordata *data) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;
  int n;

  client_sockfd = socket_connect(SENSOR_PORT, SENSOR_HOST);
  if (! command->SerializeToFileDescriptor(client_sockfd) ) {
    print_error("ERROR writing to socket");
  }
  print_sensorcommand(NET_OUT, command);

  n = read(client_sockfd, buffer, BUFFERSIZE);
  if (n > 0) {
    data->ParseFromArray(buffer, n);
    print_sensordata(NET_IN, data);
  }

  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
}

int main(int argc, char *argv[])
{
  Controller ctrl;
  messages::motorcommand *mcommand;
  messages::motorstatus *mstatus;
  messages::sensorcommand *scommand;
  messages::sensordata *sdata1;
  messages::sensordata *sdata2;

  // reset motor1
  mcommand = new messages::motorcommand();
  mstatus = new messages::motorstatus();
  mcommand->set_type(messages::motorcommand::RESET);
  mcommand->set_motor(1);
  ctrl.socket_write_motorcommand(mcommand, mstatus);

  // calibrate sensor1
  scommand = new messages::sensorcommand();
  sdata = new messages::sensordata();
  scommand->set_type(messages::sensorcommand::CALIBRATE);
  scommand->set_sensor(1);
  ctrl.socket_write_sensorcommand(scommand, sdata1);

  while (1) {
    // read sensor1 values
    scommand = new messages::sensorcommand();
    sdata1 = new messages::sensordata();
    scommand->set_type(messages::sensorcommand::GET);
    scommand->set_sensor(1);
    ctrl.socket_write_sensorcommand(scommand, sdata);

    // read sensor2 values
    scommand = new messages::sensorcommand();
    sdata2 = new messages::sensordata();
    scommand->set_type(messages::sensorcommand::GET);
    // TODO: select sensor2
    scommand->set_sensor(1);
    ctrl.socket_write_sensorcommand(scommand, sdata2);

    // sleep
    sleep(1);
  }

  return 0;
}
