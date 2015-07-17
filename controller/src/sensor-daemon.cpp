/**
  * @file sensor-daemon.cpp
  * @brief read sensor values, send them via protobuf messages
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "sensor-daemon.h"

#ifdef HOST_BBB
// pin 19,20
LSM303 mag("/dev/i2c-1");
// pin 17,18
// LSM303 mag("/dev/i2c-2");
#endif

Sensor::Sensor() :
    sensor1 {},
    sensor2 {}
{
  // initialize socket
#ifndef BBB_SENSOR2
  printf("built for SENSOR1\n");
  sockfd = socket_open(SENSOR1_PORT);
#else
  printf("built for SENSOR2\n");
  sockfd = socket_open(SENSOR2_PORT);
#endif

}

Sensor::~Sensor() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

void Sensor::handle_sensorcommand (messages::sensorcommand *command, messages::sensordata *data) {
  int s;
  // select sensor
  s = command->sensor();
  data->set_sensor(s);

  if (command->type() == messages::sensorcommand::GET) {
    // sending values anyway
  }

  else if (command->type() == messages::sensorcommand::CALIBRATE) {

    // select sensor
    if (s == 1) {
      // read current values
#ifdef HOST_BBB
      mag.readMag();
      convert_coordinates(mag.m[0], mag.m[1], mag.m[2],
          &sensor1.theta_offset, &sensor1.phi_offset);
#endif
    }
    else if (s == 2) {
      printf("TODO\n");
      // sensor2.x = 0;
      // sensor2.y = 0;
      // sensor2.z = 0;
    }
  }

  // create response
  if (s == 1) {
    // DEBUG
    // printf("response: theta %.2f - %.2f = %.2f, phi: %.2f - %.2f = %.2f\n",
    //     sensor1.theta, sensor1.theta_offset, sensor1.theta - sensor1.theta_offset,
    //     sensor1.phi, sensor1.phi_offset, sensor1.phi - sensor1.phi_offset);

    // return the current sensor position minus the saved offset
    data->set_theta(sensor1.theta - sensor1.theta_offset);
    data->set_phi(sensor1.phi - sensor1.phi_offset);
  }
  else if (s == 2) {
    printf("TODO\n");
    data->set_theta(sensor2.theta - sensor2.theta_offset);
    data->set_phi(sensor2.phi - sensor2.phi_offset);
  }

}

void Sensor::socket_read_sensorcommand () {
  int new_sockfd;
  int client_sockfd;
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

    // update values and send them to the controller
    if (t_diff > update_timeout) {
#ifdef HOST_BBB
      mag.readMag();
      convert_coordinates(mag.m[0], mag.m[1], mag.m[2], &sensor1.theta, &sensor1.phi);


      // DEBUG
      // printf("update: theta %.2f - %.2f = %.2f, phi: %.2f - %.2f = %.2f\n",
      //     sensor1.theta, sensor1.theta_offset, sensor1.theta - sensor1.theta_offset,
      //     sensor1.phi, sensor1.phi_offset, sensor1.phi - sensor1.phi_offset);
#endif

      // send updated values
      messages::sensordata *data = new messages::sensordata();
      // TODO create a function for this
      data->set_sensor(1);
      data->set_theta(sensor1.theta - sensor1.theta_offset);
      data->set_phi(sensor1.phi - sensor1.phi_offset);
      client_sockfd = socket_connect(CONTROLLER_PORT, CONTROLLER_HOST);
      if( client_sockfd < 0) {
          print_error("CONNECT error");
      }
      socket_write_sensordata(client_sockfd, data);
      shutdown(client_sockfd, 0);
      close(client_sockfd);
      gettimeofday(&tv_last, NULL);
    }

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
            messages::sensorcommand *message = new messages::sensorcommand();
            messages::sensordata *response = new messages::sensordata();

            // parse message
            message->ParseFromArray(buffer, n);
            print_sensorcommand(NET_IN, message);
            // generate response
            handle_sensorcommand(message, response);
            print_sensordata(NET_OUT, response);
            socket_write_sensordata(*it, response);
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

void Sensor::socket_write_sensordata (int sockfd, messages::sensordata *data) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);

  // serialize data
  int err = data->SerializeToArray(buffer, data->ByteSize());
  if ( ! err ) {
    printf("error: %d\n", err);
  }
  // send data
  write(sockfd, buffer, data->ByteSize());
}

int main(void) {
  Sensor snsr;

#ifdef HOST_BBB
  // initialize sensors
  mag.enable();
#endif

  // main loop
  while (1) {
    snsr.socket_read_sensorcommand();
  }

  return(0);
}
