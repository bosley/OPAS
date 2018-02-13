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

import nmap
import optparse
from socket import gethostbyname

def scan(host, port, return_type):
    scans = {
        'open':'O',
        'filtered':'F',
        'closed':'X'
    }
    nscan = nmap.PortScanner()
    nscan.scan(host, port)

    try:
        info=nscan[host]['tcp'][int(port)]
    except:
        if return_type == 0:
            response = "[Unreachable]"
        elif return_type == 1:
            response = ["Unreachable"]
        elif return_type == 2:
            response = [host, "Unreachable"]
        return response

    if return_type == 0:
        return " [ " + scans[info['state']] + " ] " + host + " tcp/" + port + " | " + info['name']
    elif return_type == 1:
        return [port, scans[info['state']], str(info['name'])]
    elif return_type == 2:
        return [host, port, scans[info['state']], str(info['name'])]

def main():
    parser = optparse.OptionParser('usage%program -H <host> -p <port>')
    parser.add_option('-H', dest='host', type='string', help='specify target')
    parser.add_option('-p', dest='port', type='string', help='specify port[s] - separated by commas')
    parser.add_option('-r', dest='portr', type='string', help='specify port range - separated by tick')
    (options,args) = parser.parse_args()

    host = options.host

    if options.port is not None and options.portr is not None:
        print "Use only -H, or -r"
        exit(0)

    if options.portr is not None:
        port_range = str(options.portr).split("-")

        try:
            r1 = int(port_range[0])
            r2 = int(port_range[1])
        except ValueError:
            print "\n\t Invalid port given for port range \n\n "
            print parser.usage
            exit(0)

        if r2 < r1:
            ports = range(r2, r1+1)
        else:
            ports = range(r1, r2+1)
    else:
        ports = str(options.port).split(',')

    if (host is None) | (ports[0] is None):
        print parser.usage
        exit(0)

    try:
        ip = gethostbyname(host)
    except:
        print "[ \ ] Unable to resolve " + host
        return

    print "[" + "-" *60 + "]"
    print "\t[ Scanning " + ip + " Please wait ]"
    print "[" + "-" *60 + "]"

    for port in ports:
        print scan(ip, str(port), 0)

if __name__ == '__main__':
    main()
