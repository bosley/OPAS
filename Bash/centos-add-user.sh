#!/bin/bash

if [ $(whoami) != 'root' ]; then
	echo "You need to be root"
	exit 1;
fi

read -p "Enter new username : " n_user

adduser $n_user
passwd $n_user

read -p "Should this user have root access? [y/n] : " u_root

if [ $u_root == "y" ]
then
	gpasswd -a $n_user wheel
fi

echo "Action complete. $n_user now exists."
