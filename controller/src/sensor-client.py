#!/usr/bin/python2
#
# sensor-client.py
#
# test the sensor-daemon
#
# Konstantin Koslowski <konstantin.koslowski@mailbox.org>
#

import socket
import time
import messages_pb2

host = socket.gethostname()
port = 2021

def sendrcv(message):
  s = socket.socket()
  s.connect((host, port))
  message_str = message.SerializeToString()
  print('message:\n%s' % message)
  s.send(message_str)

  response_str = s.recv(1024)
  response = messages_pb2.sensordata()
  response.ParseFromString(response_str)
  print('response:\n%s' % response)

  # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(.5)

def main():
  message = messages_pb2.sensorcommand()
  message.type = message.GET
  message.sensor = 1
  sendrcv(message)

  message = messages_pb2.sensorcommand()
  message.type = message.GET
  message.sensor = 2
  sendrcv(message)

  message = messages_pb2.sensorcommand()
  message.type = message.CALIBRATE
  message.sensor = 2
  sendrcv(message)

  message = messages_pb2.sensorcommand()
  message.type = message.GET
  message.sensor = 2
  sendrcv(message)

if __name__ == "__main__":
  main()

