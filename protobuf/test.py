#!/usr/bin/python2

import sensor_pb2
import sys

data = sensor_pb2.sensorData()
data.x = 1
data.y = 2
data.z = 3

print("x: %d, y: %d, z: %d" % (data.x, data.y, data.z))
