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

def sendrcv2(msg):
  s = socket.socket()
  s.connect((host, port))
  s.send(msg)
  print (s.recv(1024))
  # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(0.2)

def sendrcv(message):
  s = socket.socket()
  s.connect((host, port))
  s.send(message.SerializeToString())
  # # print (s.recv(1024))
  # # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(0.2)

def main():
  message = messages_pb2.msg()
  message.type = message.COMMAND
  message.command.type = message.command.RESET
  sendrcv(message)

  message.type = message.SENSORDATA
  sendrcv(message)

  message.type = message.MOTORSTATUS
  sendrcv(message)

  # sendrcv('reset 0')
  # while (True):
    # steps = 800
    # acc = 10
    # sendrcv('loop 1 %d %d' % (steps, acc))
    # sendrcv('loop 2 %d %d' % (steps, acc))
    # sendrcv('status')
    # sendrcv('loop 1 %d %d' % (-steps, acc))
    # sendrcv('loop 2 %d %d' % (-steps, acc))
    # sendrcv('status')

    # steps = 1000
    # acc = 10
    # sendrcv('loop 1 %d %d' % (steps, acc))
    # sendrcv('status')
    # sendrcv('loop 1 %d %d' % (-steps, acc))
    # sendrcv('status')

    # steps = 80
    # acc = 10
    # for i in range(0, 10):
    #   print('step+ %d' % i)
    #   sendrcv('loop 1 %d %d' % (steps, acc))
    #   sendrcv('status')
    # for i in range(0, 10):
    #   print('step- %d' % i)
    #   sendrcv('loop 1 %d %d' % (-steps, acc))
    #   sendrcv('status')

if __name__ == "__main__":
  main()

