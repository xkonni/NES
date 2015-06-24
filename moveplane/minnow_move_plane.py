#!/usr/bin/python2
import serial
import Adafruit_GPIO as GPIO
import time
import math
from collections import namedtuple

motor = namedtuple("motor", "step dir")
sensor = namedtuple("sensor", "dev rate")

## parameters - beaglebone
# motor1 = motor(step = "P8_8", dir = "P8_10")
# motor2 = motor(step = "P8_12", dir = "P8_14")
## parameters - minnowboard
motor1 = motor(step = 476, dir = 477)
motor2 = motor(step = 478, dir = 479)
sensor1 = sensor(dev = "/dev/ttyUSB0", rate = 9600)
sensor2 = sensor(dev = "/dev/ttyUSB1", rate = 9600)
## parameters - general
delta = 50
gpio = GPIO.get_platform_gpio()
pos_max = 70
pos_min = -70

## functions
# single step on rising edge
def step (pin):
  gpio.output(pin, GPIO.HIGH)
  time.sleep(1E-3)
  gpio.output(pin, GPIO.LOW)
  time.sleep(1E-3)

# do some steps
def loop (pin_step, pin_dir, num, dir):
  gpio.output(pin_dir, dir)
  for i in range(0, num):
    step(pin_step)

def test():
  while True:
    loop(motor1.step, motor1.dir, 70, GPIO.HIGH)
    loop(motor2.step, motor2.dir, 70, GPIO.HIGH)

    loop(motor1.step, motor1.dir, 140, GPIO.LOW)
    loop(motor2.step, motor2.dir, 140, GPIO.LOW)

    loop(motor1.step, motor1.dir, 70, GPIO.HIGH)
    loop(motor2.step, motor2.dir, 70, GPIO.HIGH)

def serialToFloat(m):
  m[0] = float(m[0])
  m[1] = float(m[1])
  m[2] = float(m[2])

def normalize(m):
  m_max = max(abs(m[0]), abs(m[1]), abs(m[2]))
  m[0] = m[0] / m_max
  m[1] = m[1] / m_max
  m[2] = m[2] / m_max

## main function
def main():
  offset_x = 0
  offset_y = 0
  offset_z = 0
  pos_x = 0
  pos_y = 0
  run = 0
  
  ## initialize serial
  serial1 = serial.Serial(sensor1.dev, sensor1.rate)
  serial2 = serial.Serial(sensor2.dev, sensor2.rate)
  
  ## initialize GPIOs
  gpio.setup(motor1.step, GPIO.OUT)
  gpio.setup(motor1.dir, GPIO.OUT)
  gpio.setup(motor2.step, GPIO.OUT)
  gpio.setup(motor2.dir, GPIO.OUT)
  
  
  # DEBUG
  # test()
  
  # run
  while True:
    data1 = serial1.readline().rstrip()
    if (len(data1) < 17):
      print('serial1: bad things happened, trying again')
      continue
  
    data2 = serial2.readline().rstrip()
    if (len(data2) < 17):
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
  
    # first 10 cycles are used to calibrate offsets between the sensors
    if run < 10:
      print("calibrating...");
      offset_x += (s1[0] - s2[0])/10
      offset_y += (s1[1] - s2[1])/10
      offset_z += (s1[2] - s2[2])/10
      run += 1
      if run == 10:
        print("calibration done")
        print("offset x:%.2f y:%.2f z:%.2f" % (offset_x, offset_y, offset_z))
  
    # update plane position
    else:
      x = s1[0] - s2[0] - offset_x
      y = s1[1] - s2[1] - offset_y
      z = s1[2] - s2[2] - offset_z
  
      nx = math.trunc(x/delta)
      ny = math.trunc(y/delta)
  
      # move x axis
      if nx > 0:
        nx = nx if pos_x + nx <= pos_max else pos_max - pos_x
        loop(motor1.step, motor1.dir, abs(nx), GPIO.HIGH)
      elif nx < 0:
        nx = nx if pos_x + nx >= pos_min else pos_min - pos_x
        loop(motor1.step, motor1.dir, abs(nx), GPIO.LOW)
      # move y axis
      if ny > 0:
        ny = ny if pos_y + ny <= pos_max else pos_max - pos_y
        loop(motor2.step, motor2.dir, abs(ny), GPIO.HIGH)
      elif ny < 0:
        ny = ny if pos_y + ny >= pos_min else pos_min - pos_y
        loop(motor2.step, motor2.dir, abs(ny), GPIO.LOW)
      pos_x = pos_x + nx
      pos_y = pos_y + ny

      print("x: %4d (%3d/%3d), y: %4d (%3d/%3d)" % (x, nx, pos_x, y, ny, pos_y))
  
    # wait for sensor data
    # time.sleep(1E-3)

if __name__ == "__main__": main()
