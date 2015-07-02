#!/usr/bin/python
import serial
import socket
import Adafruit_BBIO.GPIO as GPIO
import time
import math

# initialize serial port
ser1 = serial.Serial('/dev/ttyUSB0', 9600)
ser2 = serial.Serial('/dev/ttyUSB1', 9600)
# wrong usb ports, dont replug, reboot and pray
# ser2 = serial.Serial('/dev/ttyUSB0', 9600)
# ser1 = serial.Serial('/dev/ttyUSB1', 9600)

host = socket.gethostname()
port = 2020

def sendrcv(msg):
  s = socket.socket()
  s.connect((host, port))
  s.send(msg)
  print (s.recv(1024))
  s.close()
  # time.sleep(0.2)

def serialToFloat(m):
  m[0] = float(m[0])
  m[1] = float(m[1])
  m[2] = float(m[2])

def normalize(m):
  m_max = max(abs(m[0]), abs(m[1]), abs(m[2]))
  m[0] = m[0] / m_max
  m[1] = m[1] / m_max
  m[2] = m[2] / m_max

def main():
  offset_x = 0
  offset_y = 0
  offset_z = 0
  run = 0
  delta = 100
  ## DEBUG
  # test()

  # update plane position
  # first 10 cycles are used to calibrate offsets between the sensors
  data1 = ser1.readline().rstrip()
  data2 = ser2.readline().rstrip()
  sendrcv('reset 0')
  while True:

    data1 = ser1.readline().rstrip()
    if (len(data1) < 15):
      print('serial1: bad things happened, trying again')
      continue

    data2 = ser2.readline().rstrip()
    if (len(data2) < 15):
      print('serial2: bad things happened, trying again')
      continue

    data1 = data1.decode('utf-8')
    data2 = data2.decode('utf-8')
    # split data
    s1 = data1.split(',')
    s2 = data2.split(',')
    # convert to number
    serialToFloat(s1)
    serialToFloat(s2)

    if run < 10:
      print("calibrating...");
      offset_x += (s1[0] - s2[0])/10
      offset_y += (s1[1] - s2[1])/10
      offset_z += (s1[2] - s2[2])/10
      run += 1
      if run == 10:
        print("calibration done")
        print("offset x:%.2f y:%.2f z:%.2f" % (offset_x, offset_y, offset_z))

    else:
      x = s1[0] - s2[0] - offset_x
      y = s1[1] - s2[1] - offset_y
      z = s1[2] - s2[2] - offset_z
      # print("x: %5d/%5d - y: %5d/%5d - z:%5d/%5d" % ( s1[0], s2[0] + offset_x, s1[1], s2[1] + offset_y, s1[2], s2[2] + offset_z))

      nx = math.trunc(x/delta)
      ny = math.trunc(y/delta)
      print("x: %d (%d), y: %d (%d)" % (x, nx, y, ny))

      # move x axis
      # if abs(nx) > 1:
      #   sendrcv("loop 1 %d" % nx )

      # move y axis
      if abs(ny) > 1:
        sendrcv("loop 2 %d" % ny )

    # wait for sensor data
    run += 1
    if (run % 100 == 0):
      sendrcv("status")
    time.sleep(1E-2)

if __name__ == "__main__":
  main()
