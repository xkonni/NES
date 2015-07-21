#!/bin/bash
FILE_IN=BB-DCAN1-00A0.dts
FILE_OUT=BB-DCAN1-00A0.dtbo

echo "building $FILE_OUT"
dtc -O dtb -o $FILE_OUT -b 0 -@ $FILE_IN

if [ $? -eq 0 ]; then
  echo -e "success.\nnow run 02_install.sh"
else
  echo "fail."
fi
