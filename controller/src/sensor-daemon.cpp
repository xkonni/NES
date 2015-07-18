/**
  * @file sensor-daemon.cpp
  * @brief read sensor values, send them via protobuf messages
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "sensor-daemon.h"

#ifdef HOST_BBB
#ifndef BBB_SENSOR2
LSM303 mag("/dev/i2c-1"); // pin 19,20
#else
LSM303 mag("/dev/i2c-2"); // pin 17,18
#endif
#endif

Sensor::Sensor() :
    sensor1 {}
{
#ifndef BBB_SENSOR2
  printf("built for SENSOR1, using \"/dev/i2c-1\"\n");
  sensor1.id = 1;
  sockfd = socket_open(SENSOR1_PORT);
#else
  printf("built for SENSOR2, using \"/dev/i2c-2\"\n");
  sensor1.id = 2;
  sockfd = socket_open(SENSOR2_PORT);
#endif
}

Sensor::~Sensor() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

void Sensor::handle_sensorcommand (messages::sensorcommand *command, messages::sensordata *data) {
  data->set_sensor(sensor1.id);

  if (command->type() == messages::sensorcommand::GET) {
    // sending values anyway
  }

  else if (command->type() == messages::sensorcommand::CALIBRATE) {

    // read current values
#ifdef HOST_BBB
    mag.readMag();
    convert_coordinates(mag.m[0], mag.m[1], mag.m[2],
        &sensor1.theta_offset, &sensor1.phi_offset);
#endif
  }

  // create response
  // DEBUG
  // printf("response: theta %.2f - %.2f = %.2f, phi: %.2f - %.2f = %.2f\n",
  //     sensor1.theta, sensor1.theta_offset, sensor1.theta - sensor1.theta_offset,
  //     sensor1.phi, sensor1.phi_offset, sensor1.phi - sensor1.phi_offset);

  // return the current sensor position minus the saved offset
  data->set_theta(sensor1.theta - sensor1.theta_offset);
  data->set_phi(sensor1.phi - sensor1.phi_offset);
}

int Sensor::get_sensordatabuffer (char *buffer) {
  bzero(buffer, BUFFERSIZE);

  messages::sensordata *data = new messages::sensordata();
  data->set_sensor(sensor1.id);
  data->set_theta(sensor1.theta - sensor1.theta_offset);
  data->set_phi(sensor1.phi - sensor1.phi_offset);
  print_sensordata(NET_OUT, data);
  // serialize data
  data->SerializeToArray(buffer, data->ByteSize());
  return data->ByteSize();
}

int main(void) {
  Sensor snsr;
  struct timeval tv_now, tv_last;
  // update timeout [usec]
  int update_timeout = 1000000;
  int n;
  long int t_diff;
  char buffer[BUFFERSIZE];
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
  messages::sensorcommand *message = new messages::sensorcommand();
  messages::sensordata *response = new messages::sensordata();

#ifdef HOST_BBB
  // initialize sensors
  mag.enable();
#endif

  // initialize time
  gettimeofday(&tv_last, NULL);
  // main loop
  while (1) {
    // update time
    gettimeofday(&tv_now, NULL);
    t_diff = (tv_now.tv_usec - tv_last.tv_usec) + (tv_now.tv_sec - tv_last.tv_sec) * 1000000;
    // listen on socket
    n = socket_listen(snsr.sockfd, connected, buffer);
    if (n > 0) {
      // parse message
      message->ParseFromArray(buffer, n);
      print_sensorcommand(NET_IN, message);
      snsr.handle_sensorcommand(message, response);
    }

    // command requested response
    if (response->has_sensor()) {
      bzero(buffer, BUFFERSIZE);
      response->SerializeToArray(buffer, response->ByteSize());
      print_sensordata(NET_OUT, response);
      n = socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, response->ByteSize());
      if (n > 0) {
        print_sensordata(NET_OUT, response);
      }
    }

    // timeout
    // update values and send them to the controller
    if (t_diff > update_timeout) {
#ifdef HOST_BBB
      mag.readMag();
      convert_coordinates(mag.m[0], mag.m[1], mag.m[2],
          &snsr.sensor1.theta, &snsr.sensor1.phi);
#else
      snsr.sensor1.theta++;
      snsr.sensor1.phi++;
#endif
      // DEBUG
      // printf("timeout: %f, %f\n", snsr.sensor1.theta, snsr.sensor1.phi);
      // send updated values
      n = snsr.get_sensordatabuffer(buffer);
      socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, n);
      gettimeofday(&tv_last, NULL);
    }
  }

  return(0);
}
