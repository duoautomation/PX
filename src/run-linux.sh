#!/bin/bash

arduino-cli compile -v &&
arduino-cli upload -v &&
cat /dev/ttyACM0

