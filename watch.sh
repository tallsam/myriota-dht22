#!/bin/bash

(stty sane 115200; cat) < /dev/ttyUSB0
