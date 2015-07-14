#!/usr/bin/python2
#
# client.py
#
# client to control the stepper motor controller
# via a socket
#
# Konstantin Koslowski <konstantin.koslowski@mailbox.org>
#

import socket
import time
import messages_pb2

host = socket.gethostname()
port = 2022

# def rcv():
#   print("receiving")
#   response = s.recv(1024)
#   print("done")
#   print(response);

def send(message):
  s = socket.socket()
  s.connect((host, port))
  message_str = message.SerializeToString()
  print('message:\n%s' % message)
  s.send(message_str)

  response_str = s.recv(1024)
  response = messages_pb2.motorstatus()
  # response = messages_pb2.motorcommand()
  response.ParseFromString(response_str)
  print('response:\n%s' % response)

  # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(.5)

def main():
  message = messages_pb2.motorcommand()
  message.type = message.RESET
  message.motor = 1
  send(message)

  while(1):
    message = messages_pb2.motorcommand()
    message.type = message.LOOP
    message.motor = 1
    message.steps = 800
    message.acc = 10
    send(message)

    message = messages_pb2.motorcommand()
    message.type = message.LOOP
    message.motor = 1
    message.steps = -800
    message.acc = 10
    send(message)

    message = messages_pb2.motorcommand()
    message.type = message.STATUS
    message.motor = 1
    send(message)

    # ## MOTORCOMMAND
    # for i in range(1, 10):
    #   message = messages_pb2.motorcommand()
    #   message.type = message.LOOP
    #   message.motor = 1
    #   message.steps = 80
    #   message.acc = 10
    #   send(message)
    #
    #   message = messages_pb2.motorcommand()
    #   message.type = message.LOOP
    #   message.motor = 2
    #   message.steps = 80
    #   message.acc = 10
    #   send(message)
    #
    # for i in range(1, 10):
    #   message = messages_pb2.motorcommand()
    #   message.type = message.LOOP
    #   message.motor = 1
    #   message.steps = -80
    #   message.acc = 10
    #   send(message)
    #
    #   message = messages_pb2.motorcommand()
    #   message.type = message.LOOP
    #   message.motor = 2
    #   message.steps = -80
    #   message.acc = 10
    #   send(message)


if __name__ == "__main__":
  main()

