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

#def __init__(self):
# host = socket.gethostname()
# port = 2020
host = socket.gethostname()
port = 2020

# def rcv():
#   print("receiving")
#   response = s.recv(1024)
#   print("done")
#   print(response);

def send(message):
  s = socket.socket()
  s.connect((host, port))
  s.send(message.SerializeToString())

  # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(0.01)

def main():
  ## MOTORSTATUS
  # message = messages_pb2.motorstatus()
  # message.motor.add(id = 1, pos = 2);
  # message.motor.add(id = 3, pos = 4);

  while(1):
    ## MOTORCOMMAND
    for i in range(1, 10):
      message = messages_pb2.motorcommand()
      message.type = message.LOOP
      message.motor = 1
      message.steps = 80
      message.acc = 10
      send(message)

      message = messages_pb2.motorcommand()
      message.type = message.LOOP
      message.motor = 2
      message.steps = 80
      message.acc = 10
      send(message)

    for i in range(1, 10):
      message = messages_pb2.motorcommand()
      message.type = message.LOOP
      message.motor = 1
      message.steps = -80
      message.acc = 10
      send(message)

      message = messages_pb2.motorcommand()
      message.type = message.LOOP
      message.motor = 2
      message.steps = -80
      message.acc = 10
      send(message)


if __name__ == "__main__":
  main()

