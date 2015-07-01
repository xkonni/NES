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

s = socket.socket()
s.connect((host, port))
s.send('status')
print (s.recv(1024))
s.close()

s = socket.socket()
s.connect((host, port))
s.send('turn 0 20')
print (s.recv(1024))
s.close()

s = socket.socket()
s.connect((host, port))
s.send('status')
print (s.recv(1024))
s.close()
