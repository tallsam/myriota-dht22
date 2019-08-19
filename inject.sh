#!/bin/bash
#updater.py --id -p /dev/ttyUSB0

TIME=`date +"%s"`
echo ${TIME} | message_inject.py 003afda802
