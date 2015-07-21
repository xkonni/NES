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
  // modify steps [-800; 800] -> [0; 1600]
  command1->set_steps(deg2steps(theta_diff) + 800);


  // motor2 -> phi
  phi_diff = data1->phi() - data2->phi();
  command2->set_type(messages::motorcommand::LOOP);
  command2->set_motor(2);
  // modify steps [-800; 800] -> [0; 1600]
  command2->set_steps(deg2steps(phi_diff) + 800);
}

void Controller::move_motor(int motor, int steps, int acc) {
  // set acceleration
  messages::motorcommand *command = new messages::motorcommand();
  command->set_type(messages::motorcommand::LOOP);
  command->set_motor(motor);
  command->set_acc(acc);
  send_motorcommand(command);

  // correct from [0; 1600] to [-800; 800]
  steps = steps - 800;

  // set steps
  command->Clear();
  command->set_type(messages::motorcommand::LOOP);
  command->set_motor(motor);
  command->set_steps(steps);
  send_motorcommand(command);
}

int Controller::send_motorcommand(messages::motorcommand *command) {
  char buffer[BUFFERSIZE];
  int n;

  command->SerializeToArray(buffer, command->ByteSize());
  n = socket_write(MOTOR_PORT, MOTOR_HOST, buffer, command->ByteSize());
  if (n > 0) {
    print_motorcommand(NET_OUT, command);
  }
  return(n);
}

int Controller::send_sensorcommand(messages::sensorcommand *command) {
  char buffer[BUFFERSIZE];
  int n;

  command->SerializeToArray(buffer, command->ByteSize());
  if (command->sensor() == SENSOR1)
    n = socket_write(SENSOR1_PORT, SENSOR1_HOST, buffer, command->ByteSize());
  else
    n = socket_write(SENSOR2_PORT, SENSOR2_HOST, buffer, command->ByteSize());
  if (n > 0) {
    print_sensorcommand(NET_OUT, command);
  }
  return(n);
}

int main(int argc, char *argv[])
{
  Controller ctrl;

  int cansock = can_open();
  char buf[BUFFERSIZE];
  sprintf(buf, "foobar");
  int size=2;
  can_write(cansock, buf, size);
  return(0);

  messages::sensordata *sdata1 = new messages::sensordata();
  messages::sensordata *sdata2 = new messages::sensordata();
  messages::motorcommand *mcommand1 = new messages::motorcommand();
  messages::motorcommand *mcommand2 = new messages::motorcommand();

  struct timeval tv_now, tv_last;
  // update timeout [usec]
  int update_timeout = 1000000;
  long int t_diff;
  int n;
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
  char buffer[BUFFERSIZE];

  // calibrate sensor1
  // scommand1->set_type(messages::sensorcommand::CALIBRATE);
  // scommand1->set_sensor(SENSOR1);
  //
  // mcommand1 = new messages::motorcommand();
  // mcommand1->set_type(messages::motorcommand::RESET);
  // mcommand1->set_motor(1);

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
