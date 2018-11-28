#!/bin/bash

download_requirements(){
    echo "Entering 'dynos_build'"
    cd ./dynos_build
    git clone https://github.com/Dynamic-Network-Overlay-System/DyNOS-Node
    git clone https://github.com/Dynamic-Network-Overlay-System/DyNOS-Hub
    git clone https://github.com/Dynamic-Network-Overlay-System/DyNOS-TCP
    git clone https://github.com/Dynamic-Network-Overlay-System/DyNOS-Keys
}

setup_env(){
    echo "Setting up enviroment"
    if [ -d ./dynos_build ]; then 
        rm -rf ./dynos_build
    fi
    mkdir ./dynos_build
    download_requirements
}

if command -v git &>/dev/null; then
    echo "Git is installed!"
else
    echo "Git is required! Exiting."
    exit 1
fi

if command -v python3 &>/dev/null; then
    echo "Python is installed!"
else
    echo "Python3 is required! Exiting."
    exit 1
fi

setup_env

echo "Pulling out packages"
mv ./DyNOS-TCP/dynostcp ./
mv ./DyNOS-Keys/src/dynoskeys ./
mv ./DyNOS-Hub/src/dynoshub ./
mv ./DyNOS-Hub/src/spin_core.py ./dynos_server.py
mv ./DyNOS-Node/src/dynosnode ./
mv ./DyNOS-Node/src/spin_node.py ./example_node.py

echo "Cleaning up"
rm -rf DyNOS-Node
rm -rf DyNOS-Hub
rm -rf DyNOS-TCP
rm -rf DyNOS-Keys

cd ..

echo "Complete"
