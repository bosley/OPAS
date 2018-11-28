#!/bin/bash

if [ -d ~/.dynos_node ]; then 
    rm -rf ~/.dynos_node
fi

if [ -d ~/.dynos_core ]; then 
    rm -rf ~/.dynos_core
fi

echo "Complete"