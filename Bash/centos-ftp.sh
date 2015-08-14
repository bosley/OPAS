#!/bin/bash

if [ $(whoami) != 'root' ]; then
	echo "You need to be root"
	exit 1;
fi

yum install -y vsftpd
systemctl enable vsftpd.service

chkconfig vsftpd on

sed -i '12s/.*/anonymous_enable=NO/' /etc/vsftpd/vsftpd.conf
sed -i '82s/.*/ascii_upload_enable=YES/' /etc/vsftpd/vsftpd.conf
sed -i '83s/.*/ascii_download_enable=YES/' /etc/vsftpd/vsftpd.conf
sed -i '86s/.*/ftpd_banner=Welcome to your FTP server./' /etc/vsftpd/vsftpd.conf

echo "use_localtime=YES" >> /etc/vsftpd/vsftpd.conf

firewall-cmd --permanent --add-service=ftp
firewall-cmd --permanent --add-port=21/tcp
firewall-cmd --reload
