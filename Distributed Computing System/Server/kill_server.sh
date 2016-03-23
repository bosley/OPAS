#!/bin/bash

# Josh A. Bosley
# Simple script to find the server and kill it

kill $(ps -ef | grep "main.py" | grep -v grep | awk '{print $2}')
rm -rf *.pyc
