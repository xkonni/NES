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
 * connect to socket
 */
int socket_connect(int port, const char *hostname) {
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
      error("ERROR opening socket");

  server = gethostbyname(hostname);
  if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
      error("ERROR connecting");

  return sockfd;
}

/*
 * read from socket
 */
int socket_read (int sockfd) {
  int client_sockfd;
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  clilen = sizeof(cli_addr);
  client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (client_sockfd < 0) {
    error("ERROR on accept");
    exit(1);
  }

  messages::motorstatus *message = new messages::motorstatus();
  if (! message->ParseFromFileDescriptor(client_sockfd)) {
    error("ERROR reading from socket");
    exit(1);
  }

  int m = 0;
  for (m = 0; m < message->motor_size(); m++) {
    messages::motorstatus::motorStatusMsg motor;
    motor = message->motor(m);
    printf("motor id: %d, pos: %d\n", motor.id(), motor.pos());
  }

  shutdown(client_sockfd, 0);
  close(client_sockfd);
  return(1);
}

/*
 * write command to socket
 */
void socket_write_command (int port, const char *host, messages::motorcommand *command) {
  int n;
  char buffer[BUFFERSIZE];
  bzero(buffer, BUFFERSIZE);
  int client_sockfd;

  client_sockfd = socket_connect(port, host);
  if (! command->SerializeToFileDescriptor(client_sockfd) ) {
    error("ERROR writing to socket");
  }
  n = read(client_sockfd, buffer, BUFFERSIZE);
  printf("%d bytes read\n", n);

  messages::motorstatus *status = new messages::motorstatus();
  status->ParseFromString(buffer);

  print_motorstatus(status);

  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
}

int main(int argc, char *argv[])
{
  messages::motorcommand *command;

  command = new messages::motorcommand();
  command->set_type(messages::motorcommand::RESET);
  command->set_motor(1);
  socket_write_command(MOTOR_PORT, MOTOR_HOST, command);

  command = new messages::motorcommand();
  command->set_type(messages::motorcommand::STATUS);
  socket_write_command(MOTOR_PORT, MOTOR_HOST, command);

  // command = new messages::motorcommand();
  // command->set_type(messages::motorcommand::LOOP);
  // command->set_motor(1);
  // command->set_steps(80);
  // command->set_acc(10);
  // socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  // command = new messages::motorcommand();
  // command->set_type(messages::motorcommand::STATUS);
  // socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  // command = new messages::motorcommand();
  // command->set_type(messages::motorcommand::LOOP);
  // command->set_motor(2);
  // command->set_steps(80);
  // command->set_acc(10);
  // socket_write_command(MOTOR_PORT, MOTOR_HOST, command);

  // while (1) {
  //   int i;
  //   for(i = 1; i < 10; i++) {
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::LOOP);
  //     command->set_motor(1);
  //     command->set_steps(80);
  //     command->set_acc(10);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::LOOP);
  //     command->set_motor(2);
  //     command->set_steps(80);
  //     command->set_acc(10);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::STATUS);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //   }
  //
  //   for(i = 1; i < 10; i++) {
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::LOOP);
  //     command->set_motor(1);
  //     command->set_steps(-80);
  //     command->set_acc(10);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::LOOP);
  //     command->set_motor(2);
  //     command->set_steps(-80);
  //     command->set_acc(10);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //
  //     command = new messages::motorcommand();
  //     command->set_type(messages::motorcommand::STATUS);
  //     socket_write_command(MOTOR_PORT, MOTOR_HOST, command);
  //   }
  // }


  return 0;
}
