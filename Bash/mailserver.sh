#!/bin/bash
# Josh A. Bosley - August 6th 2015

if [ $(whoami) != 'root' ]; then
	echo "You need to be root"
	exit 1;
fi

DEBUG=false

THE_HOST=1
THE_DOMAIN=2

function retHost {
	read -p "Enter the hostname you want to assign :  " s_host
	echo $s_host
}

function retDomain {
	read -p "Enter your domain name : " s_domain
	echo $s_domain
}

# This will ask the user for information, and then verify the answer.
function getVar {

	RETVAR=""
	VERIFICATION="n"
	while [ $VERIFICATION != 'y' ]
	do 
		case "$1" in

			$THE_HOST)  RETVAR=$(retHost)
			;;

			$THE_DOMAIN)  RETVAR=$(retDomain)
			;;
	
			*)  echo "Signal number $1 is not processed"
	    		;;
		esac
	
	read -p "Is $RETVAR correct? [y/n] " VERIFICATION	
	done

	echo $RETVAR
}

s_ip="$( ip route get 8.8.8.8 | awk 'NR==1 {print $NF}' )"
s_host=$(getVar $THE_HOST)
s_domain=$(getVar $THE_DOMAIN)
host_entry="$s_ip $s_host"

# Set host information
if $DEBUG
then
	echo $host_entry >> test_host
	echo "Script is in DEBUG mode. Can not continue."
	exit
else
	hostnamectl set-hostname $s_host
	echo $host_entry >> /etc/hosts
fi
clear

APPEND="
myhostname = $s_host
mydomain = $s_domain
myorigin = $s_domain
home_mailbox = mail/
mynetworks = 127.0.0.0/8
inet_interfaces = all
mydestination = $myhostname, localhost.$mydomain, localhost, $mydomain
smtpd_sasl_type = dovecot
smtpd_sasl_path = private/auth
smtpd_sasl_local_domain =
smtpd_sasl_security_options = noanonymous
broken_sasl_auth_clients = yes
smtpd_sasl_auth_enable = yes
smtpd_recipient_restrictions = permit_sasl_authenticated,permit_mynetworks,reject_unauth_destination
smtp_tls_security_level = may
smtpd_tls_security_level = may
smtp_tls_note_starttls_offer = yes
smtpd_tls_loglevel = 1
smtpd_tls_key_file = /etc/postfix/ssl/server.key
smtpd_tls_cert_file = /etc/postfix/ssl/server.crt
smtpd_tls_received_header = yes
smtpd_tls_session_cache_timeout = 3600s
tls_random_source = dev:/dev/urandom
"

cat <<EOT >> /etc/postfix/main.cf
$APPEND
EOT

echo "Host set"
echo "Installing postfix, dovecot"
yum -y install postfix dovecot

echo "Making ssl key"
mkdir /etc/postfix/ssl
cd /etc/postfix/ssl
openssl genrsa -des3 -out server.key 2048
openssl rsa -in server.key -out server.key.insecure
mv server.key server.key.secure
mv server.key.insecure server.key
clear

echo ""
echo "Please leave the challenge password blank. Press eneter when prompted."
echo ""

openssl req -new -key server.key -out server.csr
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

# Insert settings at line 15 in /etc/postfix/master.cf
sed -i "15i submission     inet  n       -       n       -       -       smtpd\n  -o syslog_name=postfix/submission\n  -o smtpd_sasl_auth_enable=yes\n  -o smtpd_recipient_restrictions=permit_sasl_authenticated,reject\n  -o milter_macro_daemon_name=ORIGINATING\nsmtps     inet  n       -       n       -       -       smtpd\n  -o syslog_name=postfix/smtps\n  -o smtpd_sasl_auth_enable=yes\n  -o smtpd_recipient_restrictions=permit_sasl_authenticated,reject\n  -o milter_macro_daemon_name=ORIGINATING\n" /etc/postfix/master.cf

echo "Checking new configuration . . . " 
postfix check

echo "Configuring . . ."
sed -i "95i # Postfix smtp-auth\nunix_listener /var/spool/postfix/private/auth {\nmode = 0660\nuser = postfix\ngroup = postfix\n}" /etc/dovecot/conf.d/10-master.conf

sed -i '100s/.*/auth_mechanisms = plain login/' /etc/dovecot/conf.d/10-auth.conf

echo "Restarting, and enabling services . . ."
systemctl restart postfix
systemctl enable postfix
systemctl restart dovecot
systemctl enable dovecot

echo "Adding firewall rules . . . "
firewall-cmd --permanent --add-service=smtp
firewall-cmd --permanent --add-port=587/tcp
firewall-cmd --permanent --add-port=465/tcp
firewall-cmd --permanent --add-port=25/tcp
firewall-cmd --reload

sed -i '30s/.*/mail_location = maildir:~\/mail/' /etc/dovecot/conf.d/10-mail.conf
sed -i '50s/.*/pop3_uidl_format = %08Xu%08Xv/' /etc/dovecot/conf.d/20-pop3.conf

systemctl restart dovecot

firewall-cmd --permanent --add-port=110/tcp
firewall-cmd --permanent --add-service=pop3s
firewall-cmd --permanent --add-port=143/tcp
firewall-cmd --permanent --add-service=imaps
firewall-cmd --reload

