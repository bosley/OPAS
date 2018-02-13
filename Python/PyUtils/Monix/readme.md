
### Anybody is welcome to use/ modify.

Uses python-nmap

	sudo apt-get install python-nmap


<b> Use of monix.py </b>

1. Create a scan settings file

	Single-Host:

		ip-range:127.0.0.1
		port-range:22
		output-file:single-scan-test.txt

	Multiple-Host:

		ip-range:127.0.0.1,127.0.0.9
		port-range:20,40
		output-file:multiple-scan-test.txt

2. Initiate the program

	python monix.py -s scan-settings

