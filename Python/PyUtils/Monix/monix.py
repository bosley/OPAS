#!/usr/bin/env python

"""
The MIT License (MIT)

Copyright (c) 2015 Josh Bosley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

__author__ = 'Josh Allen Bosley'

import os
import Queue
import optparse
import threading
import subprocess
import nmap

queue = Queue.Queue()

class ThreadedScan(threading.Thread):

    def __init__(self, queue, port_range, output_folder):
        threading.Thread.__init__(self)
        self.queue = queue
        self.port_range = port_range
        self.output_folder = output_folder
        self.scans = {
        'open':'O',
        'filtered':'F',
        'closed':'X'
        }

    def run(self):
        while True:
            ip = self.queue.get()

            # Create file for output of this scan
            output_file = self.output_folder + "/" + ip

            with open(output_file, 'w') as of:
                of.write('')

            p = subprocess.Popen(
                ["ping", "-c", "1", ip],
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE
            )
            out, r = p.communicate()

            # Make sure the address is active
            if not r:
                port_information = ""

                # Scan each port for each ip
                for port in self.port_range:
                    nscan = nmap.PortScanner()
                    nscan.scan(ip, str(port))
                    try:
                        info = nscan[ip]['tcp'][port]
                        information = [port, self.scans[info['state']], str(info['name'])]

                        # Add port information to string for output
                        port_information += "\n"
                        for el in information:
                            port_information += str(el)
                            port_information += ","
                        port_information = port_information[:-1]
                        port_information += "\n"

                    except:
                        port_information += "unreachable\n"

                # Write out all ports data retrieved after every port scanned
                with open(output_file, 'a') as output_file:
                    output_file.write(port_information)
            else:
                with open(output_file + "-unreachable", 'w') as of:
                    of.write('')

            # Completed
            self.queue.task_done()

def generate_ip_range(start_ip, end_ip):
    start = list(map(int, start_ip.split(".")))
    end = list(map(int, end_ip.split(".")))
    temp = start
    ip_range = []
    ip_range.append(start_ip)
    while temp != end:
        start[3] += 1
        for i in (3, 2, 1):
            if temp[i] == 256:
                temp[i] = 0
                temp[i-1] += 1
        ip_range.append(".".join(map(str, temp)))
    return ip_range

def load_file_to_dictionary(dict, file):
    f = open(file, 'r')
    for l in f.readlines():
        if l.strip():
            data = l.split(":")
            if len(data) < 2:
                print "\n\t Error with " + file + "\n Line [" + l + "]"
            dict[data[0].strip()] = data[1].strip()
    f.close()

def main():
    parser = optparse.OptionParser('\nusage%program -s <scan settings> \n')
    parser.add_option('-s', dest='setting', type='string', help='Scan settings file')
    (options,args) = parser.parse_args()

    if (options.setting is None):
        print parser.usage
        exit(0)

    print "\n\t Nynja:Monix"

    # Dictionaries used to store settings
    monix_setup = {}
    scan_settings = {}

    scan_settings['monix-setup'] = "req/monix-setup"
    scan_settings['settings-file'] = options.setting.strip()

    # Make sure the settings file was specified and can be loaded
    if not os.path.isfile(scan_settings['settings-file']):
        print "\n\t Could not find settings file [ "  + scan_settings['settings-file'] + " ]\n"
        print parser.usage
        exit(0)

    # Load the settings
    load_file_to_dictionary(monix_setup, scan_settings['monix-setup'])
    load_file_to_dictionary(scan_settings, scan_settings['settings-file'])

    # Build range of ips to scan
    ip_range = []
    if len(scan_settings['ip-range'].split(',')) == 1:
        ip_range.append(scan_settings['ip-range'].strip())
    else:
        start_ip = scan_settings['ip-range'].split(',')[0].strip()
        end_ip = scan_settings['ip-range'].split(',')[1].strip()
        ip_range = generate_ip_range(start_ip, end_ip)

    # Build range of ports to scan
    port_range = []
    if len(scan_settings['port-range'].split(',')) == 1:
        port_range.append(scan_settings['port-range'].strip())
    else:
        try:
            start_port = int(scan_settings['port-range'].split(',')[0].strip())
            end_port = int(scan_settings['port-range'].split(',')[1].strip())
        except ValueError:
            print "Invalid ports given in [" + scan_settings['settings-file'] + "]"
        except:
            print "Error generating port range"
        if end_port < start_port:
            port_range = range(end_port, start_port+1)
        else:
            port_range = range(start_port, end_port+1)

    print "\n\t Initiating scans. Please wait. \n"

    # Test if destination folder exists
    if not os.path.exists(scan_settings['output-file']):
        os.makedirs(scan_settings['output-file'])

    # Create pool of threads, with queue
    for i in range(int(monix_setup['scan-pool'])):
        t = ThreadedScan(queue, port_range, scan_settings['output-file'])
        t.setDaemon(True)
        t.start()

    # Load the queue
    for ip in ip_range:
        queue.put(ip)

    # Wait for processing to get done
    queue.join()

    print "\n\t Complete \n"

if __name__ == '__main__':
    main()
