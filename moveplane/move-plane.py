#!/usr/bin/python
import serial
import Adafruit_BBIO.GPIO as GPIO
import time

# initialize serial port
ser = serial.Serial('/dev/ttyUSB0', 9600)

# initialize GPIOs
GPIO.setup("P8_8", GPIO.OUT)
GPIO.setup("P8_10", GPIO.OUT)
GPIO.setup("P8_12", GPIO.OUT)
GPIO.setup("P8_14", GPIO.OUT)

# some variables
prev_x = 0
prev_y = 0
prev_z = 0
delta = 0.01

# update plane position
while True:
  data = ser.readline().rstrip()
  if (len(data) < 5):
    print('bad things happened, try again')
    exit(1)

  data = data.decode('utf-8')
  # split serial data
  sx, sy, sz = data.split(',')
  # convert to number
  x = float(sx)
  y = float(sy)
  z = float(sz)
  # calculate max
  m = max(abs(x), abs(y), abs(z))
  # normalize to 1
  x = round(x/m, 2)
  y = round(y/m, 2)
  z = round(z/m, 2)
  print('x: %.2f y: %.2f z: %.2f' % (x, y, z))

  # x axis
  n = int((prev_x - x)/delta)
  for i in range (0, n):
    print("inc x")
    GPIO.output("P8_10", GPIO.LOW)
    GPIO.output("P8_8",  GPIO.HIGH)
    time.sleep(1E-6)
    GPIO.output("P8_8",  GPIO.LOW)
  n = int((x - prev_x)/delta)
  for i in range (0, n):
    print("dec x")
    GPIO.output("P8_10", GPIO.HIGH)
    GPIO.output("P8_8",  GPIO.HIGH)
    GPIO.output("P8_8",  GPIO.LOW)

  # y axis
  n = int((prev_y - y)/delta)
  for i in range (0, n):
    print("inc y")
    GPIO.output("P8_14", GPIO.LOW)
    GPIO.output("P8_12", GPIO.HIGH)
    GPIO.output("P8_12", GPIO.LOW)
  n = int((y - prev_y)/delta)
  for i in range (0, n):
    print("dec y")
    GPIO.output("P8_14", GPIO.HIGH)
    GPIO.output("P8_12", GPIO.HIGH)
    GPIO.output("P8_12", GPIO.LOW)

  prev_x = x
  prev_y = y
  prev_z = z
  time.sleep(1E-3)
