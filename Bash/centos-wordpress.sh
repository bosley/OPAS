#!/bin/bash

if [ $(whoami) != 'root' ]; then
	echo "You need to be root"
	exit 1;
fi

echo "Set up database. . . "

read -p "Enter a password for Database wordpressuser : " wp_pass

echo ""
echo " You are about to be prompted for the database root password "

mysql -u root -p << EOF
CREATE DATABASE wordpress;
CREATE USER wordpressuser@localhost IDENTIFIED BY "$wp_pass";
GRANT ALL PRIVILEGES ON wordpress.* TO wordpressuser@localhost IDENTIFIED BY '$wp_pass';
FLUSH PRIVILEGES;
exit
EOF

# Get php ready
yum install php php-gd php-mysql -assumeyes

systemctl httpd restart

# Get the lastes wordpress, and rsynv to public dir
cd ~
wget http://wordpress.org/latest.tar.gz
tar xzvf latest.tar.gz
sudo rsync -avP ~/wordpress/ /var/www/html/

# Make uploads area, and setup to configure installation
mkdir /var/www/html/wp-content/uploads
sudo chown -R apache:apache /var/www/html/*
cd /var/www/html

cp wp-config-sample.php wp-config.php

# Edit settings
sed -i '19s/.*/define( 'DB_NAME',     	'wordpres' );/' wp-config.php
sed -i '22s/.*/define( 'DB_USER',     	'wordpressuser' );/' wp-config.php
sed -i '35s/.*/define( 'DB__PASSWORD',  '$wp_pass' );/' wp-config.php

echo ""
echo "  Must complete setup via webpage"
echo ""





