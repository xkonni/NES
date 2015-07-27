/**
  * @file sensor-daemon.cpp
  * @brief read sensor values, send them via protobuf messages
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "sensor-daemon.h"

#ifdef BBB_HOST
#ifndef BBB_SENSOR2
LSM303 lsm303("/dev/i2c-1"); // pin 19,20
#else
LSM303 lsm303("/dev/i2c-2"); // pin 17,18
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
    // TODO: we may need to compress the values by making sure they're positive
    // data->set_x((sensor1.x - sensor1.x_offset + 360) % 360);
    // data->set_y((sensor1.y - sensor1.y_offset + 360) % 360);
    // TODO: do that later
    data->set_x(sensor1.x - sensor1.x_offset);
    data->set_y(sensor1.y - sensor1.y_offset);
  }
  /*
   * CALIBRATE sensor
   */
  else if (command->type() == messages::sensorcommand::CALIBRATE) {
    // read current values
    // TODO: use average values
#ifdef BBB_HOST
    // use MAG
    // lsm303.readMag();
    // convert_coordinates(lsm303.m[0], lsm303.m[1], lsm303.m[2],
    //     &sensor1.x_offset, &sensor1.y_offset);
    // use ACC
    lsm303.readAcc();
    convert_coordinates(lsm303.a[0], lsm303.a[1], lsm303.a[2],
        &sensor1.x_offset, &sensor1.y_offset);
#endif
  }

  // DEBUG
  // printf("calibrated: x %d - %d = %d, y: %d - %d = %d\n",
  //     sensor1.x, sensor1.x_offset, sensor1.x - sensor1.x_offset,
  //     sensor1.y, sensor1.y_offset, sensor1.y - sensor1.y_offset);
}

int Sensor::get_sensordatabuffer (char *buffer) {
  bzero(buffer, BUFFERSIZE);

  messages::sensordata *data = new messages::sensordata();
  data->set_sensor(sensor1.id);
  // TODO: we may need to compress the values by making sure they're positive
  // data->set_x((sensor1.x - sensor1.x_offset + 360) % 360);
  // data->set_y((sensor1.y - sensor1.y_offset + 360) % 360);
  // TODO: do that later
  data->set_x(sensor1.x - sensor1.x_offset);
  data->set_y(sensor1.y - sensor1.y_offset);
  print_sensordata(NET_OUT, data);
  // serialize data
  data->SerializeToArray(buffer, data->ByteSize());
  return data->ByteSize();
}

int Sensor::sample(int sample_idx, int *samples) {
  int i;

  // DEBUG
  // printf("sampling...");
#ifdef BBB_HOST
  // use ACC
  lsm303.readAcc();
  samples[sample_idx] = lsm303.a[0];
  samples[NUM_SAMPLES + sample_idx] = lsm303.a[1];
#else
  sensor1.x++;
  sensor1.y++;
#endif
  // DEBUG
  // printf("sample %d, %d\n", samples[sample_idx], samples[NUM_SAMPLES + sample_idx]);
  sample_idx = (sample_idx + 1) % NUM_SAMPLES;

  if (sample_idx == 0) {
    // update values to average of sampled data
    // DEBUG
    // printf("updating values...");
    sensor1.x = 0;
    sensor1.y = 0;
    for (i = 0; i < NUM_SAMPLES; i++) {
      sensor1.x += samples[i];
      sensor1.y += samples[NUM_SAMPLES + i];
    }
    sensor1.x = sensor1.x / NUM_SAMPLES;
    sensor1.y = sensor1.y / NUM_SAMPLES;
    // DEBUG
    // printf("value %d, %d\n", sensor1.x, sensor1.y);
  }

  return(sample_idx);
}



int main(void) {
  Sensor snsr;
  struct timeval tv_now, tv_last_sample;
  long int t_diff_sample;
  int *samples = (int *)malloc(2*NUM_SAMPLES*sizeof(int));
  int sample_idx;
  int n;
  char buffer[BUFFERSIZE];
#ifndef BBB_CAN
  // connected clients
  std::vector<int> *connected = new std::vector<int>();
#endif
  messages::sensorcommand *message = new messages::sensorcommand();
  messages::sensordata *response = new messages::sensordata();

#ifdef BBB_HOST
  // initialize sensors
  lsm303.enable();
#endif

  // initialize time
  gettimeofday(&tv_last_sample, NULL);
  sample_idx = 0;
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
    t_diff_sample = (tv_now.tv_usec - tv_last_sample.tv_usec)
        + (tv_now.tv_sec - tv_last_sample.tv_sec) * 1000000;

    // sample sensor values, maybe update
    if (t_diff_sample > SAMPLE_TIMEOUT) {
      sample_idx = snsr.sample(sample_idx, samples);

      if (sample_idx == 0) {
        // send updated values
        n = snsr.get_sensordatabuffer(buffer);
#ifdef BBB_CAN
        can_write(snsr.sockfd, CAN_SENSORDATA, buffer, n);
#else
        socket_write(CONTROLLER_PORT, CONTROLLER_HOST, buffer, n);
#endif
      }
      gettimeofday(&tv_last_sample, NULL);
    }
  }

  return(0);
}
