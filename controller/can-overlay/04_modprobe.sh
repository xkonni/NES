#!/bin/bash
MODULES="can can-dev can-raw"
for m in $MODULES; do
  echo "loading $m"
  sudo modprobe $m
  if [ $? -eq 0 ]; then
    echo "success.\nnow run 05_set.sh'.
  else
    echo "fail."
    break
  fi
done
