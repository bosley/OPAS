#! /bin/bash
#
#	Requires dnf 
#

declare -A dnfinstalls

dnfinstalls=(
	  ["mc"]="dnf install mc --assumeyes"
	  ["htop"]="dnf install htop --assumeyes" 
	  ["iptraf"]="dnf install iptraf --assumeyes" 
	  ["w3m"]="dnf install w3m --assumeyes"
	  ["vim"]="dnf install vim --assumeyes"
	  ["filezilla"]="dnf install filezilla --assumeyes"
	  ["openjdk"]="dnf install java --assumeyes"
	 )

echo "[ Performing Installs ]"

for k in "${!dnfinstalls[@]}"
do		
	echo "Installing : " $k
	${dnfinstalls[$k]}
done

echo "Getting Developer tools"
dnf groupinstall "Development tools" --assumeyes

echo "Installing Flash"
rpm -ivh http://linuxdownload.adobe.com/adobe-release/adobe-release-x86_64-1.0-1.noarch.rpm
rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-adobe-linux
dnf install flash-plugin nspluginwrapper alsa-plugins-pulseaudio libcurl --assumeyes
dnf groupinstall "Sound and Video" --assumeyes
dnf install flash-plugin nspluginwrapper curl --assumeyes

echo "[ Updating ]"
dnf update --assumeyes





