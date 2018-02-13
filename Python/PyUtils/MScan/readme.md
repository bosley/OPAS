
### Anybody is welcome to use/ modify.

Uses python-nmap

	sudo apt-get install python-nmap

<b> Use of mscan.py </b>

Usage:

	Single Port:

		mscan.py -H 127.0.0.1 -p 21

	Multiple Port:

		mscan.py -H 127.0.0.1 -p 21,22,23,24

	Range Port:

		mscan.py -H 127.0.0.1 -r 20-100

	Name, or IP may be given in all uses :

		mscan.py -H localhost -r 20-100
