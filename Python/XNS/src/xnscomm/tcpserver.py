#!/usr/local/bin/python3

from time import sleep

import threading
import socketserver

from xnsdb import DataStore
from xnscommon import settings
from .transmission import NetworkTransmitter
from .xnsrequesthandler import XNSRequestHandler

#	TCP Socket Server
#
#
#
class TCPConnectionHandler(socketserver.BaseRequestHandler):

	def handle(self):
    		
		# Begin client validation - If they don't have the key, close conn
		if(self.transmitter.decode(self.request.recv(settings["verification_block"])) != settings["handshake_key"]):
			self.request.close()
			return
		else:
			# If they have the key, send them some data to know
			# that they can continue
			self.request.send(self.transmitter.encode(settings["accepted"]))
		
		#  Expecting a message size from the client (num of blocks)		
		size = self.transmitter.decode(self.request.recv(settings["verification_block"]))
		
		# Try converting the size to an int, if we can't error to them
		# and close the connection. Might be a spy
		try:
			size = int(size)
		except:
			# Just in case something funny happened 
			try:
				self.request.send(self.transmitter.encode(settings["error"]))	
			except:
				pass
			self.request.close()
			return

		# Tell client that we are ready to receive their data
		self.request.send(self.transmitter.encode(settings["accepted"]))

		# Receive the chunks of data from the client
		chunks = []
		bytes_recd = 0
		while bytes_recd < size:
			chunk = self.request.recv(min(size - bytes_recd, 2048))
			if chunk == b'':
				self.ds.addLogEntry(["REQUEST_SERVER", "Emptry Request"])
			chunks.append(chunk)
			bytes_recd = bytes_recd + len(chunk)
		
		# Translate the message to plain text
		data = self.transmitter.decode(b"".join(chunks))

		# Inform the client that we got all of their data
		self.request.send(self.transmitter.encode(settings["accepted"]))	

		# Get the reply by analyzing the data. Send the data manager for any
		# potential inquiries or requests
		reply = self.xnsReqHandler.analyze(data)

		# Encode response and send data size
		reply = self.transmitter.encode(reply)
		self.request.send(self.transmitter.encode(str(len(reply))))

		# Get verification that the size is accepted
		try:
			verification = self.request.recv(settings["verification_block"])
		except:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed to verify that the size was accepted"])
			return -1

		if self.transmitter.decode(verification) != settings["accepted"]:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed attempt to send response size to requester"])
			return -1

		# Send the data
		total = 0
		while total < len(reply):
			sent = self.request.send(reply[total:])
			if 0 == sent:
				self.ds.addLogEntry(["REQUEST_SERVER", "Socket connection broken"])
			total = total + sent

		# Check to make sure the request was sent
		try:
			verification = self.request.recv(settings["verification_block"])
		except:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed attempt to ensure req was sent. - Shutdown req?"])
			return -2

		if self.transmitter.decode(verification) != settings["accepted"]:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed attempt to send response to requester"])
			return -2

		# Close transmission
		self.request.close()

# The threaded TCP socket server
#
#
#
class Server(socketserver.ThreadingMixIn, socketserver.TCPServer):

	daemon_threads = True
	allow_reuse_address = True

	def __init__(self, address, RequestHandlerClass, _deviceManager):

		# Give the handler class a request handler, and give access to device manager thread
		# as-well-as the nexus thread
		RequestHandlerClass.xnsReqHandler = XNSRequestHandler(_deviceManager)

		# Create a data store instance
		RequestHandlerClass.ds = DataStore()

		# Give it a transmitter
		RequestHandlerClass.transmitter = NetworkTransmitter(settings)

		# init server
		socketserver.TCPServer.__init__(self, address, RequestHandlerClass)

	def serve(self):
		self.handle_request()

	def force_stop(self):
		self.server_close()
		self.signal = False
		
class serverMonitor(threading.Thread):
	def __init__(self, _deviceManager):
		threading.Thread.__init__(self)
		self.signal = True
		self.serv = Server( 
			(settings["address"], settings["port"]), 
			TCPConnectionHandler, 
			_deviceManager)

	def run(self):
		while self.signal:
			self.serv.serve()
		self.serv.force_stop()
			


