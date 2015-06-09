#!/usr/bin/python
import matplotlib.pyplot as plt
import serial
import numpy as np
import time


## setup
# initialize serial port
ser = serial.Serial('/dev/ttyUSB0', 9600)
# set plot to animated
plt.ion()
# initialize x and y vectors
xdata = [0] * 2
ydata = [0] * 2
# ax = plt.axes()
plt.title('heading')

line, = plt.plot(xdata, ydata, 'r')
plt.axis([-1, 1, -1, 1])
# plt.show()

while True:
  data = ser.readline().rstrip()
  if (len(data) < 10):
    print('bad things happened, trying again')
    continue

  angle = float(data.decode('utf-8'))
  xdata[1] = np.cos(angle/180 * np.pi)
  ydata[1] = np.sin(angle/180 * np.pi)
  print('%.2f' % angle)

  line.set_xdata(xdata)
  line.set_ydata(ydata)
  plt.draw()
  plt.pause(100E-6)
