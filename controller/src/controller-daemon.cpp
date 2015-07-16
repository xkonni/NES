/*
 * controller.cpp
 *
 * control motors and sensors, each running as a separate process,
 * communicate via protobuf messages
 *
 * Konstantin Koslowski <konstantin.koslowski@mailbox.org>
 */
#include "controller-daemon.h"

/*
 * print error and exit
 */
void error(const char *reply) {
  perror(reply);
  exit(1);
}

/*
 * write motorcommand to socket
 */
void socket_write_motorcommand (messages::motorcommand *command) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;

  client_sockfd = socket_connect(MOTOR_PORT, MOTOR_HOST);
  if (! command->SerializeToFileDescriptor(client_sockfd) ) {
    error("ERROR writing to socket");
  }
  read(client_sockfd, buffer, BUFFERSIZE);
  // int n = read(client_sockfd, buffer, BUFFERSIZE);
  // printf("%d bytes read\n", n);

  messages::motorstatus *status = new messages::motorstatus();
  status->ParseFromString(buffer);

  print_motorcommand(NET_OUT, command);
  print_motorstatus(NET_IN, status);

  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
}

/*
 * write sensorcommand to socket
 */
void socket_write_sensorcommand (messages::sensorcommand *command) {
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;

  client_sockfd = socket_connect(SENSOR_PORT, SENSOR_HOST);
  if (! command->SerializeToFileDescriptor(client_sockfd) ) {
    error("ERROR writing to socket");
  }
  read(client_sockfd, buffer, BUFFERSIZE);
  // int n = read(client_sockfd, buffer, BUFFERSIZE);
  // printf("%d bytes read\n", n);

  messages::sensordata *data = new messages::sensordata();
  data->ParseFromString(buffer);

  print_sensorcommand(NET_OUT, command);
  print_sensordata(NET_IN, data);
  std::vector<double> *sph_coord = new std::vector<double>();
  convert_sensordata(data, sph_coord);

  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
}

int main(int argc, char *argv[])
{
  messages::motorcommand *mcommand;
  messages::sensorcommand *scommand;

  // // reset motor1
  // mcommand = new messages::motorcommand();
  // mcommand->set_type(messages::motorcommand::RESET);
  // mcommand->set_motor(1);
  // socket_write_motorcommand(mcommand);

  // // reset motor2
  // mcommand = new messages::motorcommand();
  // mcommand->set_type(messages::motorcommand::RESET);
  // mcommand->set_motor(2);
  // socket_write_motorcommand(mcommand);

  // // calibrate sensor1
  // scommand = new messages::sensorcommand();
  // scommand->set_type(messages::sensorcommand::CALIBRATE);
  // scommand->set_sensor(1);
  // socket_write_sensorcommand(scommand);

  // // calibrate sensor2
  // scommand = new messages::sensorcommand();
  // scommand->set_type(messages::sensorcommand::CALIBRATE);
  // scommand->set_sensor(2);
  // socket_write_sensorcommand(scommand);

  while (1) {
    // read sensor values
    scommand = new messages::sensorcommand();
    scommand->set_type(messages::sensorcommand::GET);
    scommand->set_sensor(1);
    socket_write_sensorcommand(scommand);

    sleep(1);

    // scommand = new messages::sensorcommand();
    // scommand->set_type(messages::sensorcommand::GET);
    // scommand->set_sensor(2);
    // socket_write_sensorcommand(scommand);

    // // rotate motor
    // // ... forward
    // mcommand = new messages::motorcommand();
    // mcommand->set_type(messages::motorcommand::LOOP);
    // mcommand->set_motor(1);
    // mcommand->set_steps(100);
    // mcommand->set_acc(10);
    // socket_write_motorcommand(mcommand);
    //
    // mcommand = new messages::motorcommand();
    // mcommand->set_type(messages::motorcommand::LOOP);
    // mcommand->set_motor(2);
    // mcommand->set_steps(100);
    // mcommand->set_acc(10);
    // socket_write_motorcommand(mcommand);
    //
    // // ... and backward
    // mcommand = new messages::motorcommand();
    // mcommand->set_type(messages::motorcommand::LOOP);
    // mcommand->set_motor(1);
    // mcommand->set_steps(-100);
    // mcommand->set_acc(10);
    // socket_write_motorcommand(mcommand);
    //
    // mcommand = new messages::motorcommand();
    // mcommand->set_type(messages::motorcommand::LOOP);
    // mcommand->set_motor(2);
    // mcommand->set_steps(-100);
    // mcommand->set_acc(10);
    // socket_write_motorcommand(mcommand);
  }

  return 0;
}
