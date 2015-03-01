#!/bin/bash
FILE=BB-DCAN1-00A0.dtbo

echo "copying $FILE to /lib/firmware"
sudo cp $FILE /lib/firmware

if [ $? -eq 0 ]; then
  echo -e "success.\nnow run activate.sh"
else
  echo "fail."
fi
