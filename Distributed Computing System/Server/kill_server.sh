#!/bin/bash

# Josh A. Bosley
# Simple script to find the server and kill it

kill $(ps -ef | grep "dcs_main.py" | grep -v grep | awk '{print $2}')
rm -rf *.pyc
