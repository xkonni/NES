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

void Controller::socket_write_motorcommand (
    messages::motorcommand *command, messages::motorstatus *status) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;
  int n;

  client_sockfd = socket_connect(MOTOR_PORT, MOTOR_HOST);
  // TODO:
  //
  // char buffer[BUFFERSIZE];
  // bzero(buffer, BUFFERSIZE);
  //
  // // serialize data
  // int err = data->SerializeToArray(buffer, data->ByteSize());
  // if ( ! err ) {
  //   printf("error: %d\n", err);
  // }
  // // send data
  // write(sockfd, buffer, data->ByteSize());
  //

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
  command1->set_motor(1);
  command1->set_steps(deg2steps(theta_diff));
  command1->set_acc(10);


  // motor2 -> phi
  phi_diff = data1->phi() - data2->phi();
  command2->set_motor(2);
  command2->set_steps(deg2steps(phi_diff));
  command2->set_acc(10);
}

void Controller::socket_write_sensorcommand (int sensor,
    messages::sensorcommand *command, messages::sensordata *data) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;
  int n;

  if (sensor == 1) {
    client_sockfd = socket_connect(SENSOR1_PORT, SENSOR1_HOST);
  } else {
    client_sockfd = socket_connect(SENSOR2_PORT, SENSOR2_HOST);
  }
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
  // messages::sensorcommand *scommand;
  // messages::sensordata *sdata1;
  // messages::sensordata *sdata2;


  int new_sockfd;
  // fds to monitor
  fd_set read_fds,write_fds;
  // timeout {[sec], [usec]}
  struct timeval waitd = {0, 100};
  struct timeval tv_now, tv_last;
  // sensor update interval [usec]
  int update_timeout = 100000;
  long int t_diff;
  int sel;
  int max_fd;
  int n;
  std::vector<int> connected;
  char buffer[BUFFERSIZE];
  // store the connecting address and size
  struct sockaddr_storage their_addr;
  socklen_t their_addr_size;

  // initialize time
  gettimeofday(&tv_last, NULL);
  while (1) {
    gettimeofday(&tv_now, NULL);
    t_diff = (tv_now.tv_usec - tv_last.tv_usec) + (tv_now.tv_sec - tv_last.tv_sec) * 1000000;

    // listen to sensorstatus messages
    // printf("listening...\n");
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    // add sockfd
    FD_SET(ctrl.sockfd, &read_fds);
    FD_SET(ctrl.sockfd, &write_fds);
    max_fd = ctrl.sockfd;

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
      if(FD_ISSET(ctrl.sockfd, &read_fds)) {
        // printf("data on sockfd\n");
        their_addr_size = sizeof(their_addr);
        new_sockfd = accept(ctrl.sockfd, (struct sockaddr*)&their_addr, &their_addr_size);
        if( new_sockfd < 0) {
            print_error("accept error");
        }
        socket_setnonblock(new_sockfd);
        connected.push_back(new_sockfd);
      }
      for (std::vector<int>::iterator it = connected.begin(); it != connected.end(); it++) {
        if (FD_ISSET(*it, &read_fds)) {
          // skip this fd
          if (*it == ctrl.sockfd) continue;
          bzero(buffer, BUFFERSIZE);
          n = read(*it, buffer, sizeof(buffer));
          // data available
          if (n > 0) {
            // message, response
            messages::sensordata *message = new messages::sensordata();

            // parse message
            message->ParseFromArray(buffer, n);
            print_sensordata(NET_IN, message);
            // generate response
            // handle_motorcommand(message, response);
            // print_motorstatus(NET_OUT, response);
            // socket_write_motorstatus(*it, response);
          }
          // client disconnected
          else if (n == 0) {
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

    // do other things
    // printf("doing other things...\n");
    gettimeofday(&tv_now, NULL);
    if (t_diff > update_timeout) {
      // mcommand = new messages::motorcommand();
      // mstatus = new messages::motorstatus();
      // mcommand->set_type(messages::motorcommand::LOOP);
      // mcommand->set_motor(1);
      // mcommand->set_steps(10);
      // mcommand->set_acc(10);
      // ctrl.socket_write_motorcommand(mcommand, mstatus);
      gettimeofday(&tv_last, NULL);
    }

  } // while (1)

  return 0;
}
  // leftovers
  // // reset motor1
  // mcommand = new messages::motorcommand();
  // mstatus = new messages::motorstatus();
  // mcommand->set_type(messages::motorcommand::RESET);
  // mcommand->set_motor(1);
  // ctrl.socket_write_motorcommand(mcommand, mstatus);
  //
  // // calibrate sensor1
  // scommand = new messages::sensorcommand();
  // sdata1 = new messages::sensordata();
  // scommand->set_type(messages::sensorcommand::CALIBRATE);
  // scommand->set_sensor(1);
  // ctrl.socket_write_sensorcommand(SENSOR1, scommand, sdata1);
  //
  // // calibrate sensor1
  // scommand = new messages::sensorcommand();
  // sdata2 = new messages::sensordata();
  // scommand->set_type(messages::sensorcommand::CALIBRATE);
  //   // TODO: select sensor2
  // scommand->set_sensor(1);
  // ctrl.socket_write_sensorcommand(SENSOR2, scommand, sdata2);

    // // request sensor1 values
    // scommand = new messages::sensorcommand();
    // sdata1 = new messages::sensordata();
    // scommand->set_type(messages::sensorcommand::GET);
    // scommand->set_sensor(1);
    // ctrl.socket_write_sensorcommand(SENSOR1, scommand, sdata1);
    //
    // // request sensor2 values
    // scommand = new messages::sensorcommand();
    // sdata2 = new messages::sensordata();
    // scommand->set_type(messages::sensorcommand::GET);
    // // TODO: select sensor2
    // scommand->set_sensor(1);
    // ctrl.socket_write_sensorcommand(SENSOR2, scommand, sdata2);
