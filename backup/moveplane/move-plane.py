#!/usr/bin/python
import serial
import Adafruit_BBIO.GPIO as GPIO
import time
import math

# initialize serial port
# ser1 = serial.Serial('/dev/ttyUSB0', 9600)
# ser2 = serial.Serial('/dev/ttyUSB1', 9600)
# wrong usb ports, dont replug, reboot and pray
ser2 = serial.Serial('/dev/ttyUSB0', 9600)
ser1 = serial.Serial('/dev/ttyUSB1', 9600)

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
delta = 100

# single step on rising edge
def step (pin):
  GPIO.output(pin, GPIO.HIGH)
  time.sleep(1E-3)
  GPIO.output(pin, GPIO.LOW)
  time.sleep(1E-3)

# do some steps
def loop (pin, num):
  # upper limit
  num = max(num, 20)
  for i in range(0, num):
    step(pin)

offset_x = 0
offset_y = 0
offset_z = 0
run = 0

def test():
  while True:
    GPIO.output("P8_10", GPIO.LOW)
    for i in range(1, 50):
      step("P8_8")

    GPIO.output("P8_10", GPIO.HIGH)
    for i in range(1, 50):
      step("P8_8")

    GPIO.output("P8_14", GPIO.LOW)
    for i in range(1, 50):
      step("P8_12")

    GPIO.output("P8_14", GPIO.HIGH)
    for i in range(1, 50):
      step("P8_12")

def serialToFloat(m):
  m[0] = float(m[0])
  m[1] = float(m[1])
  m[2] = float(m[2])

def normalize(m):
  m_max = max(abs(m[0]), abs(m[1]), abs(m[2]))
  m[0] = m[0] / m_max
  m[1] = m[1] / m_max
  m[2] = m[2] / m_max

## DEBUG
# test()

# update plane position
# first 10 cycles are used to calibrate offsets between the sensors
data1 = ser1.readline().rstrip()
data2 = ser2.readline().rstrip()
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
    if nx > 1:
      GPIO.output("P8_10", GPIO.HIGH)
      loop("P8_8", abs(nx))
      # step("P8_8")
    elif nx < -1:
      GPIO.output("P8_10", GPIO.LOW)
      loop("P8_8", abs(nx))
      # step("P8_8")

    # move y axis
    #if ny > 1:
    #  GPIO.output("P8_14", GPIO.LOW)
    #  loop("P8_12", abs(ny))
    #  # step("P8_12")
    #elif ny < 1:
    #  GPIO.output("P8_14", GPIO.HIGH)
    #  loop("P8_12", abs(ny))
    #  # step("P8_12")

  # wait for sensor data
  time.sleep(1E-2)
