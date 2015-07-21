#!/bin/bash
sudo ip link set can0 up type can bitrate 125000 && sudo ifconfig can0 up
if [ $? -eq 0 ]; then
  echo -e "success.\neverything done."
else
  echo "fail."
fi
