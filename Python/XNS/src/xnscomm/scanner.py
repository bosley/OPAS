import os
import socket
import subprocess
import multiprocessing

import netifaces as ni
from json import loads as jloads

from xnsdb import DataStore
from xnscommon import settings
from .xnsrequesthandler import probeInfo
from .transmission import NetworkTransmitter

# Check to see if ip is a recognizable device
def identifyDevice(ip, ds):
	
	nodes = []
	node_ip = ni.ifaddresses(settings["adapter"])[2][0]['addr']
	print("Checking ip : " + ip, end='\t')

	if node_ip == ip:
		print('\033[92m' + "[THIS NODE]" + '\033[0m', end=' -> ')
		return 1
	else:
		transmitter = NetworkTransmitter(settings)
		try:
			# This won't work if the devices aren't accepting traffic
			# Sends the probe request string from tcpserver
			resp = transmitter.outgoingQuery(ip, settings["port"], "Q$" + probeInfo(True))
		except:
			resp = settings["error"]

		# If we got a response from them
		if resp != settings["error"]:
			ds.addNodeActivity(jloads(resp))
			print('\033[92m' + "[Node Logged]" + '\033[0m', end=' -> ')
			return 1
		else:
			return 0

# Ping the ip, and add to results if it i reached
def reach(job_q, results_q):
	NUL = open(os.devnull, 'w')
	while True:
		ip = job_q.get()
		if ip is None:
			break
		try:
			subprocess.check_call(['ping', '-c 1', '-i 0.1', ip], stdout=NUL)
			results_q.put(ip)
			print("Device found [" + ip + "]")
		except:
			pass

# Look for other nodes on the network
def scanForNodes(ip, ds):

	try:
		pool_size = int(settings["scan_pool"])
	except:
		print("[Invalid scan_pool in settings")
		exit(1)

	# ips, and hits queue
	ips = multiprocessing.Queue()
	hits = multiprocessing.Queue()

	# Create a pool of processes to ping,
	# giving them the queues for IPC
	pool = [ multiprocessing.Process(target=reach, args=(ips,hits)) for i in range(pool_size) ]

	for p in pool:
		p.start()

	# Generate Addresses
	for r in range(0, 256):
		ips.put(ip[0] + "." + ip[1] + "." + ip[2] + "." + str(r))

	# Poison Pill
	for p in pool:
		ips.put(None)

	for p in pool:
		p.join()

	while not hits.empty():
		if 0 == identifyDevice(hits.get(), ds):
			print("... " + '\033[91m' + "NO RESP" + '\033[0m')
		else:
			print("... " + '\033[92m' + "RESP" + '\033[0m')

# Start setup
def setup():

	# This node's IP
	node_ip = ni.ifaddresses(settings["adapter"])[2][0]['addr']

	# A data store for the scan procedure
	ds = DataStore()
	
	# Warning that the ip on file isn't the ip leased
	if settings["address"] != node_ip:
		print('\033[93m' + "\tWARNING: Ip in settings does not match current ip"
				+ ".\n\t\tIP on file: " + settings["address"] 
				+ " | IP on device [" + settings["adapter"] + "]" 
				+ ": " + node_ip)

	print('\033[92m' + "Starting scan for nodes and network devices" + '\033[0m')
	print('\033[92m' + "This node ip : " + '\033[0m' + '\033[93m' + node_ip + '\033[0m')		
	chunks = node_ip.split('.')
	scanForNodes(chunks, ds)

def manual(address):
	NUL = open(os.devnull, 'w')
	try:
		subprocess.check_call(['ping', '-c 1', '-i 0.1', address], stdout=NUL)
		print("Device found [" + address + "]")
	except:
		print("No device found")
		return
	ds = DataStore()

	if 0 == identifyDevice(address, ds):
		print("[NO RESP]")
	else:
		print("[RESP]")
	