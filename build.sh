#!/bin/bash

make clean;
make;
updater.py -u tempSensor.bin -p /dev/ttyUSB0 -s

if [ "$1" == "make" ]; then 
  exit;
fi

# Initialise the board with the lat/lng
# echo "Setting time, latitude and longitude"
# echo "`date +%s`,-31.9825,115.884444" > /dev/ttyUSB0

if [ "$1" != "nocat" ]; then
#  satellite_simulator.py&
  (stty sane 115200; cat) < /dev/ttyUSB0
fi
