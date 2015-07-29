/**
  * @file sensor-daemon.cpp
  * @brief read sensor values, send them via protobuf messages
  *
  * @author Konstantin Koslowski <konstantin.koslowski@mailbox.org>
  */

#include "sensor-daemon.h"

#ifdef BBB_HOST
LSM303 lsm303("/dev/i2c-1"); // pin 19,20
// LSM303 lsm303("/dev/i2c-2"); // pin 17,18
#endif

Sensor::Sensor() :
    sensor1 {}
{
#ifndef BBB_SENSOR2
  printf("built for SENSOR1, using \"/dev/i2c-1\"\n");
  sensor1.id = 1;
#else
  printf("built for SENSOR2, using \"/dev/i2c-1\"\n");
  sensor1.id = 2;
#endif
#ifdef BBB_CAN
  sockfd = can_open();
#else
  sockfd = socket_open(SENSOR1_PORT);
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
    // TODO: we may need to compress the values by making sure they're positive
    // data->set_theta((sensor1.theta - sensor1.theta_offset + 360) % 360);
    // data->set_phi((sensor1.phi - sensor1.phi_offset + 360) % 360);
    // TODO: do that later
    data->set_theta(sensor1.theta - sensor1.theta_offset);
    data->set_phi(sensor1.phi - sensor1.phi_offset);
  }
  /*
   * CALIBRATE sensor
   */
  else if (command->type() == messages::sensorcommand::CALIBRATE) {
    // read current values
#ifdef BBB_HOST
    // use MAG
    // lsm303.readMag();
    // convert_coordinates(lsm303.m[0], lsm303.m[1], lsm303.m[2],
    //     &sensor1.theta_offset, &sensor1.phi_offset);
    // use ACC
    lsm303.readAcc();
    convert_coordinates(lsm303.a[0], lsm303.a[1], lsm303.a[2],
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
  // TODO: we may need to compress the values by making sure they're positive
  // data->set_theta((sensor1.theta - sensor1.theta_offset + 360) % 360);
  // data->set_phi((sensor1.phi - sensor1.phi_offset + 360) % 360);
  // TODO: do that later
  data->set_theta(sensor1.theta - sensor1.theta_offset);
  data->set_phi(sensor1.phi - sensor1.phi_offset);
  print_sensordata(NET_OUT, data);
  // serialize data
  data->SerializeToArray(buffer, data->ByteSize());
  return data->ByteSize();
}

int Sensor::sample(int sample_idx, int *samples) {
  int i;
  int theta, phi;
  int last_theta = samples[sample_idx];
  int last_phi = samples[NUM_SAMPLES + sample_idx];

#ifdef BBB_HOST
  lsm303.readAcc();
  convert_coordinates(lsm303.a[0], lsm303.a[1], lsm303.a[2], &theta, &phi);

  // DEBUG
  // printf("t %d/%d, p %d/%d\n", theta, last_theta, phi, last_phi);
  // theta increased
  if (theta > last_theta) {
    if (theta - last_theta < MAX_SAMPLE_DIFF)
      samples[sample_idx] = theta;
    else
      samples[sample_idx] = last_theta + MAX_SAMPLE_DIFF;
  // theta decreased
  } else {
    if (last_theta - theta < MAX_SAMPLE_DIFF)
      samples[sample_idx] = theta;
    else
      samples[sample_idx] = last_theta - MAX_SAMPLE_DIFF;
  }

  // phi increased
  if (phi > last_phi) {
    if (phi - last_phi < MAX_SAMPLE_DIFF)
      samples[NUM_SAMPLES + sample_idx] = phi;
    else
      samples[NUM_SAMPLES + sample_idx] = last_phi + MAX_SAMPLE_DIFF;
  // phi decreased
  } else {
    if (last_phi - phi < MAX_SAMPLE_DIFF)
      samples[NUM_SAMPLES + sample_idx] = phi;
    else
      samples[NUM_SAMPLES + sample_idx] = last_phi - MAX_SAMPLE_DIFF;
  }
  // DEBUG
  // printf("t %d/%d: %d, p %d/%d: %d\n",
  // theta, last_theta, samples[sample_idx],
  // phi, last_phi, samples[NUM_SAMPLES + sample_idx]);
#else
  sensor1.theta++;
  sensor1.phi++;
#endif

  // increment index
  sample_idx = (sample_idx + 1) % NUM_SAMPLES;

  // array filled, update saved values
  if (sample_idx == 0) {
    // DEBUG
    // last_theta = sensor1.theta;
    // last_phi = sensor1.phi;
    sensor1.theta = 0;
    sensor1.phi = 0;
    for (i = 0; i < NUM_SAMPLES; i++) {
      sensor1.theta += samples[i];
      sensor1.phi += samples[NUM_SAMPLES + i];
    }
    sensor1.theta = sensor1.theta / NUM_SAMPLES;
    sensor1.phi = sensor1.phi / NUM_SAMPLES;
    // DEBUG
    // printf("theta: %d -> %d: %d, phi: %d -> %d: %d\n",
    //     last_theta, sensor1.theta, last_theta - sensor1.theta,
    //     last_phi, sensor1.phi, last_phi - sensor1.phi);
  }

  return(sample_idx);
}

int main(void) {
  Sensor snsr;
  struct timeval tv_now, tv_last_sample;
  long int t_diff_sample;
  int samples[2*NUM_SAMPLES];
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

  // initialize samples to 90deg
  std::fill_n(samples, 2*NUM_SAMPLES, 90);

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
      message->ParsePartialFromArray(buffer, n);
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
    if (t_diff_sample > TIMEOUT_SENSOR_SAMPLE) {
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
