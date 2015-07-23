#!/bin/bash

if [ $(whoami) != 'root' ]; then
	echo "You need to be root"
	exit 1;
fi

echo " Please stay at the CLI while setup takes place."
echo " It will require interaction"

yum install httpd -y
systemctl start httpd.service
systemctl enable httpd.service
yum install mariadb-server mariadb -y
systemctl start mariadb
clear

echo "DB ROOT PASSWORD NOT SET, PRESS ENTER WHEN PROMPTED AND SET"
read -p "Press ENTER to continue."

mysql_secure_installation
systemctl enable mariadb.service
yum install php -y
clear

echo "Setup Ip tables, and open port 80"
read -p "Press ENTER to continue."

yum install -y iptables-services
systemctl enable iptables.service
iptables -I INPUT 4 -p tcp -m state --state NEW -m tcp --dport 80 -j ACCEPT
/usr/libexec/iptables/iptables.init save

echo "Allow folders/files in /var/www/html to be accessed."

restorecon -r /var/www/html
