/**
  * @file sensor-daemon.cpp
  * @brief read sensor values, send them via protobuf messages
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "sensor-daemon.h"

#ifdef BBB_HOST
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
#ifdef BBB_CAN
  sockfd = can_open();
#else
  sockfd = socket_open(SENSOR1_PORT);
#endif
#else
  printf("built for SENSOR2, using \"/dev/i2c-2\"\n");
  sensor1.id = 2;
#ifdef BBB_CAN
  sockfd = can_open();
#else
  sockfd = socket_open(SENSOR2_PORT);
#endif
#endif
}

Sensor::~Sensor() {
  // shutdown
  shutdown(sockfd, 0);
  close(sockfd);
}

void Sensor::handle_sensorcommand (messages::sensorcommand *command, messages::sensordata *data) {
  data->set_sensor(sensor1.id);

  /*
   * GET current values
   */
  if (command->type() == messages::sensorcommand::GET) {
    // return the current sensor position minus the saved offset
    data->set_theta((sensor1.theta - sensor1.theta_offset + 360) % 360);
    data->set_phi((sensor1.phi - sensor1.phi_offset + 360) % 360);
  }
  /*
   * CALIBRATE sensor
   */
  else if (command->type() == messages::sensorcommand::CALIBRATE) {
    // read current values
#ifdef BBB_HOST
    mag.readMag();
    convert_coordinates(mag.m[0], mag.m[1], mag.m[2],
        &sensor1.theta_offset, &sensor1.phi_offset);
#endif
  }

  // DEBUG
  // printf("calibrated: theta %d - %d = %d, phi: %d - %d = %d\n",
  //     sensor1.theta, sensor1.theta_offset, sensor1.theta - sensor1.theta_offset,
  //     sensor1.phi, sensor1.phi_offset, sensor1.phi - sensor1.phi_offset);
}

int Sensor::get_sensordatabuffer (char *buffer) {
  bzero(buffer, BUFFERSIZE);

  messages::sensordata *data = new messages::sensordata();
  data->set_sensor(sensor1.id);
  data->set_theta((sensor1.theta - sensor1.theta_offset + 360) % 360);
  data->set_phi((sensor1.phi - sensor1.phi_offset + 360) % 360);
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
#ifndef BBB_CAN
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
#endif
  messages::sensorcommand *message = new messages::sensorcommand();
  messages::sensordata *response = new messages::sensordata();

#ifdef BBB_HOST
  // initialize sensors
  mag.enable();
#endif

  // messages::sensordata *datafoo = new messages::sensordata();
  // printf("initialize size: %d\n", datafoo->ByteSize());
  // datafoo->set_sensor(1);
  // printf("sensor size: %d\n", datafoo->ByteSize());
  // datafoo->set_theta(0);
  // printf("theta 0 %d size: %d\n", datafoo->theta(), datafoo->ByteSize());
  // datafoo->set_theta(360);
  // printf("theta 360 %d size: %d\n", datafoo->theta(), datafoo->ByteSize());
  // datafoo->set_phi(0);
  // printf("phi 0 %d size: %d\n", datafoo->phi(), datafoo->ByteSize());
  // datafoo->set_phi(360);
  // printf("phi 360 %d size: %d\n", datafoo->phi(), datafoo->ByteSize());

  // messages::motorcommand *commandfoo = new messages::motorcommand();
  // printf("initialize size: %d\n", commandfoo->ByteSize());
  // commandfoo->set_type(messages::motorcommand::LOOP);
  // commandfoo->set_motor(1);
  // printf("type & motor size: %d\n", commandfoo->ByteSize());
  // commandfoo->set_steps(0);
  // printf("steps 0 %d size: %d\n", commandfoo->steps(), commandfoo->ByteSize());
  // commandfoo->set_steps(1600);
  // printf("steps 1600 %d size: %d\n", commandfoo->steps(), commandfoo->ByteSize());

  // exit(0);

  // initialize time
  gettimeofday(&tv_last, NULL);
  // main loop
  while (1) {
    /*
     * I) listen on socket
     */
#ifdef BBB_CAN
    n = can_listen(snsr.sockfd, CAN_SENSORCOMMAND, buffer);
#else
    n = socket_listen(snsr.sockfd, connected, buffer);
#endif
    if (n > 0) {
      // initialize message, response
      message->Clear();
      response->Clear();

      // parse message
      message->ParseFromArray(buffer, n);
      print_sensorcommand(NET_IN, message);
      snsr.handle_sensorcommand(message, response);
    }

    /*
     * II) command requested response
     */
    if (response->has_sensor()) {
      printf("sending response\n");
      bzero(buffer, BUFFERSIZE);
      response->SerializeToArray(buffer, response->ByteSize());
#ifdef BBB_CAN
      n = can_write(snsr.sockfd, CAN_SENSORDATA, buffer, response->ByteSize());
#else
      n = socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, response->ByteSize());
#endif
      if (n > 0) {
        print_sensordata(NET_OUT, response);
        printf("bytesize: %d\n", response->ByteSize());
      }
      response->Clear();
    }

    // update time
    gettimeofday(&tv_now, NULL);
    t_diff = (tv_now.tv_usec - tv_last.tv_usec) + (tv_now.tv_sec - tv_last.tv_sec) * 1000000;
    // timeout
    // update values and send them to the controller
    if (t_diff > update_timeout) {
      // update last
      gettimeofday(&tv_last, NULL);
#ifdef BBB_HOST
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
#ifdef BBB_CAN
      can_write(snsr.sockfd, CAN_SENSORDATA, buffer, n);
#else
      socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, n);
#endif
    }
  }

  return(0);
}
