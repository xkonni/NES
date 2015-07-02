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
#	host = socket.gethostname()
#	port = 2020
host = socket.gethostname()
port = 2020

def sendrcv(msg):
	s = socket.socket()
	s.connect((host, port))
	s.send(msg)
	print (s.recv(1024))
	s.close()
	time.sleep(0.2)
	
def main():
  sendrcv('status')
  sendrcv('loop 1 -10')
  sendrcv('reset 0')
  while (True):
		sendrcv('loop 1 140')
		sendrcv('status')
		# sendrcv('loop 2 140')
		# sendrcv('status')
		sendrcv('loop 1 -140')
		sendrcv('status')
		# sendrcv('loop 2 -140')
		# sendrcv('status')

if __name__ == "__main__":
  main()

