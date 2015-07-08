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

#def __init__(self):
# host = socket.gethostname()
# port = 2020
host = socket.gethostname()
port = 2020

def sendrcv(msg):
  s = socket.socket()
  s.connect((host, port))
  s.send(msg)
  print (s.recv(1024))
  # shutdown and disallow RD and WR access
  s.shutdown(socket.SHUT_RDWR)
  s.close()
  time.sleep(0.2)
  
def main():
  sendrcv('reset 0')
  while (True):
    steps = 100
    acc = 10;
    sendrcv('loop 1 %d %d' % (steps, acc))
    sendrcv('status')
    sendrcv('loop 1 %d %d' % (-steps, acc))
    sendrcv('status')

if __name__ == "__main__":
  main()

