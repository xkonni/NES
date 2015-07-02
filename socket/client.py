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

host = socket.gethostname()
port = 2020

def sendrcv(msg):
  s = socket.socket()
  s.connect((host, port))
  s.send(msg)
  print (s.recv(1024))
  s.close()
  
sendrcv('status')
sendrcv('turn 20')
sendrcv('status')
sendrcv('turn 20')
sendrcv('status')
sendrcv('turn 20')
sendrcv('status')
sendrcv('turn 20')
sendrcv('status')

sendrcv('turn -20')
sendrcv('status')
sendrcv('turn -20')
sendrcv('status')
sendrcv('turn -20')
sendrcv('status')
sendrcv('turn -20')
sendrcv('status')
