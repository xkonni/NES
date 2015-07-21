#!/bin/bash
echo BB-DCAN1 | sudo tee /sys/devices/bone_capemgr.*/slots

if [ $? -eq 0 ]; then
  dmesg | tail -n 15
  echo -e "success.\nnow run 04_modprobe.sh"
else
  echo "fail."
fi
