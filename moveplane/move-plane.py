#!/usr/bin/python
import serial
import Adafruit_BBIO.GPIO as GPIO
import time

# initialize serial port
#ser1 = serial.Serial('/dev/ttyUSB0', 9600)
#ser2 = serial.Serial('/dev/ttyUSB1', 9600)
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
delta = 0.03

# single step on rising edge
def step (pin):
  GPIO.output(pin, GPIO.HIGH)
  time.sleep(1E-6)
  GPIO.output(pin, GPIO.LOW)

# do some steps
def loop (pin, num):
  # num = max(num, 10)
  for i in range(0, num):
    step(pin)

offset_x = 0
offset_y = 0
offset_z = 0
run = 0

# update plane position
# first 10 cycles are used to calibrate offsets between the sensors
while True:
  data1 = ser1.readline().rstrip()
  if (len(data1) < 10):
    print('serial1: bad things happened, trying again')
    continue
  data2 = ser2.readline().rstrip()
  if (len(data2) < 10):
    print('serial2: bad things happened, trying again')
    continue

  data1 = data1.decode('utf-8')
  data2 = data2.decode('utf-8')
  # split serial data1
  sx1, sy1, sz1 = data1.split(',')
  sx2, sy2, sz2 = data2.split(',')
  # convert to number
  x1 = float(sx1)
  x2 = float(sx2)
  y1 = float(sy1)
  y2 = float(sy2)
  z1 = float(sz1)
  z2 = float(sz2)
  # calculate max
  m1 = max(abs(x1), abs(y1), abs(z1))
  m2 = max(abs(x2), abs(y2), abs(z2))
  # normalize to 1
  x1 = round(x1/m1, 2)
  x2 = round(x2/m2, 2)
  y1 = round(y1/m1, 2)
  y2 = round(y2/m2, 2)
  z1 = round(z1/m1, 2)
  z2 = round(z2/m2, 2)

  if run < 10:
    print("calibrating...");
    offset_x += (x1 - x2)/10
    offset_y += (y1 - y2)/10
    offset_z += (z1 - z2)/10
    run += 1
    if run == 10:
      print("calibration done")
      print("offset x:%.2f y:%.2f z:%.2f" % (offset_x, offset_y, offset_z))

  else:
    x = x1 - x2 - offset_x
    y = y1 - y2 - offset_y
    z = z1 - z2 - offset_z
    #print("x: %.2f - y: %.2f - z:%.2f" % ( x, y, z ))

    # move x axis
    n = int(x/delta)
    if n > 0:
      print("inc x: %d" % (n))
      GPIO.output("P8_10", GPIO.LOW)
      loop("P8_8", abs(n))
    elif n < 0:
      print("dec x: %d" % (abs(n)))
      GPIO.output("P8_10", GPIO.HIGH)
      loop("P8_8", abs(n))

    # move y axis
    n = int(y/delta)
    if n < 0:
      print("inc y: %d" % (n))
      GPIO.output("P8_14", GPIO.LOW)
      loop("P8_12", abs(n))
    elif n > 0:
      print("dec y: %d" % (abs(n)))
      GPIO.output("P8_14", GPIO.HIGH)
      loop("P8_12", abs(n))

  # wait for sensor data1
  time.sleep(1E-3)
