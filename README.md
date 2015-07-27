INSTRUCTIONS
============

dependencies
------------

  ```
  apt-get install i2c-tools libi2c-dev libprotobuf-dev protobuf-compiler
  ```

code
----
* clone this repository

  ```
  git clone https://github.com/xkonni/NES.git
  ```

* initialize gitmodules

  ```
  git submodule init
  ```

* update gitmodules

  ```
  git submodule update
  ```

* change to build directory

  ```
  cd controller
  ```


* build BBBIOlib

  ```
  cd src/lib/BBBIOlib
  make
  cd -
  ```

* if using can

  ```
  export CAN=1
  ```

* run cmake

  ```
  cmake .
  ```

* run make

  ```
  make

TODO
====

controller
----------
* optional: read motorstatus
* improve calibration
  * e.g. make user turn to 0, to -90, to +90, read sensor values, update deg2steps()

sensor
------
* sample data N times and store average value
* same for calibration

motor
-----
* make sure we're turning in correct direction
