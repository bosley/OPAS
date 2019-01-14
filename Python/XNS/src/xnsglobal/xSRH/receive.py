'''
The MIT License (MIT)

Copyright (c) 2017 Josh Bosley

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
'''

from time import sleep

import threading
import socketserver
from .send import send
from .handle import handler
from .DataStore import DataStore

#	TCP Socket Server
#
#
#
class TCPConnectionHandler(socketserver.BaseRequestHandler):

	def handle(self):
    		
		# Begin client validation - If they don't have the key, close conn
		if(self.transmitter.decode(self.request.recv(self.settings["verification_block"])) != self.settings["handshake_key"]):
			self.request.close()
			return
		else:
			# If they have the key, send them some data to know
			# that they can continue
			self.request.send(self.transmitter.encode(self.settings["accepted"]))
		
		#  Expecting a message size from the client (num of blocks)		
		size = self.transmitter.decode(self.request.recv(self.settings["verification_block"]))
		
		# Try converting the size to an int, if we can't error to them
		# and close the connection. Might be a spy
		try:
			size = int(size)
		except:
			# Just in case something funny happened 
			try:
				self.request.send(self.transmitter.encode(self.settings["error"]))	
			except:
				pass
			self.request.close()
			return

		# Tell client that we are ready to receive their data
		self.request.send(self.transmitter.encode(self.settings["accepted"]))

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
		self.request.send(self.transmitter.encode(self.settings["accepted"]))	

		# Get the reply by analyzing the data. Send the data manager for any
		# potential inquiries or requests
		reply = self.reqHandler.handle(data)

		# Encode response and send data size
		reply = self.transmitter.encode(reply)
		self.request.send(self.transmitter.encode(str(len(reply))))

		# Get verification that the size is accepted
		try:
			verification = self.request.recv(self.settings["verification_block"])
		except:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed to verify that the size was accepted"])
			return -1

		if self.transmitter.decode(verification) != self.settings["accepted"]:
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
			verification = self.request.recv(self.settings["verification_block"])
		except:
			self.ds.addLogEntry(["REQUEST_SERVER", "Failed attempt to ensure req was sent. - Shutdown req?"])
			return -2

		if self.transmitter.decode(verification) != self.settings["accepted"]:
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

	def __init__(self, address, RequestHandlerClass, settings, handlerRedirect):

		RequestHandlerClass.reqHandler = handler(settings, handlerRedirect)

		RequestHandlerClass.ds = DataStore()

		RequestHandlerClass.transmitter = send(settings)

		RequestHandlerClass.settings = settings

		socketserver.TCPServer.__init__(self, address, RequestHandlerClass)

	def serve(self):
		self.handle_request()

	def force_stop(self):
		self.server_close()
		self.signal = False
		
class serverThread(threading.Thread):
	def __init__(self, settings, handleMethod):
		threading.Thread.__init__(self)
		self.signal = True
		self.serv = Server( 
			(settings["address"], settings["port"]), 
			TCPConnectionHandler, 
			settings, handleMethod)

	def run(self):
		while self.signal:
			self.serv.serve()
		self.serv.force_stop()
			


