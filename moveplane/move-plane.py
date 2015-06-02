#!/usr/bin/python
import serial
import Adafruit_BBIO.GPIO as GPIO
import time

# initialize serial port
ser = serial.Serial('/dev/ttyUSB0', 9600)

# initialize GPIOs
# Motor1 step
GPIO.setup("P8_8", GPIO.OUT)
# Motor1 direction
GPIO.setup("P8_10", GPIO.OUT)
# Motor2 step
GPIO.setup("P8_12", GPIO.OUT)
# Motor2 direction
GPIO.setup("P8_14", GPIO.OUT)

# some variables
prev_x = 0
prev_y = 0
prev_z = 0
delta = 0.02

# single step on rising edge
def step (pin):
  GPIO.output(pin, GPIO.HIGH)
  time.sleep(1E-6)
  GPIO.output(pin, GPIO.LOW)

# do some steps
def loop (pin, num):
  for i in range(0, num):
    step(pin)

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
  # print('x: %.2f y: %.2f z: %.2f' % (x, y, z))

  # x axis
  n = int((x - prev_x)/delta)
  if n > 0:
    print("inc x: %d" % (n))
    GPIO.output("P8_10", GPIO.LOW)
    loop("P8_8", n)
  elif n < 0:
    print("dec x: %d" % (abs(n)))
    GPIO.output("P8_10", GPIO.HIGH)
    loop("P8_8", abs(n))

  # y axis
  n = int((y - prev_y)/delta)
  if n > 0:
    print("inc y: %d" % (n))
    GPIO.output("P8_14", GPIO.LOW)
    loop("P8_12", n)
  elif n < 0:
    print("dec y: %d" % (abs(n)))
    GPIO.output("P8_14", GPIO.HIGH)
    loop("P8_12", abs(n))

  prev_x = x
  prev_y = y
  prev_z = z

  # wait for sensor data
  time.sleep(1E-3)
