# Log Used
import time

# Communications Required Libs
import socket
import base64
from Crypto.Cipher import AES
from xnsdb import DataStore

class NetworkTransmitter:
    	
	def __init__(self, config):
		self.config = config
		self.ds = DataStore()

	def logMessage(self, module, message):
		self.ds.addLogEntry([module,message])

	# Pad the data for en/decrypting
	def padding(self, data, b):
		if(b < len(data)):
			b += b
			return self.padding(data, b)
		else:
			return data.rjust(b)

	# Encode the data for network transmissions
	def encode(self, data):
		data = self.padding(data, 16)
		cipher = AES.new(self.config["encryption_key"],AES.MODE_ECB)
		return base64.b64encode(cipher.encrypt(data))

	# Decode data received from a network transmission
	def decode(self, data):
		data = self.padding(data, 16)
		cipher = AES.new(self.config["encryption_key"],AES.MODE_ECB)
		data = cipher.decrypt(base64.b64decode(data))
		return data.decode("utf-8").lstrip()

	# Send the server a message
	def outgoingQuery(self, address, port, query):
		
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.settimeout(self.config["network_transmission_timeout"])
		try:
			sock.connect((address, port))
		except:
			sock.close()
			return self.config["error"]
		
		# Send handshake request
		sock.sendall(self.encode(self.config["handshake_key"]))
		verification = sock.recv(self.config["verification_block"])

		if self.decode(verification) != self.config["accepted"]:
			self.logMessage("TRANSMISSION","Failed attempt to handshake server")
			return -1
		
		# Encode the query
		query = self.encode(query)

		# Send size of request to server
		sock.sendall(self.encode(str(len(query))))

		# Ensure server is ready
		verification = sock.recv(self.config["verification_block"])

		if self.decode(verification) != self.config["accepted"]:
			self.logMessage("TRANSMISSION","Failed attempt to send query size to server")
			return -2
			
		# Send the data
		total = 0
		while total < len(query):
			sent = sock.send(query[total:])
			if 0 == sent:
				self.logMessage("TRANSMISSION","Socket connection broken")
			total = total + sent
			
		# Check to make sure that the server got the request	
		verification = sock.recv(self.config["verification_block"])
		if self.decode(verification) != self.config["accepted"]:
			self.logMessage("TRANSMISSION","Failed attempt to send query to server")
			return -3

		# Request sent, now get the response
		# Start by getting the size of the request
		try:
			size = self.decode(sock.recv(self.config["verification_block"]))
		except:
			sock.sendall(self.encode(self.config["error"]))
			sock.close()
			return

		try:
			size = int(size)
		except:
			self.logMessage("TRANSMISSION","Couldn't verify size of request from server")
			sock.sendall(self.encode(self.config["error"]))
			sock.close()
			return

		# Inform the server that we accept their given size
		sock.sendall(self.encode(self.config["accepted"]))

		# Receive the data
		chunks = []
		bytes_recd = 0
		while bytes_recd < size:
			chunk = sock.recv(min(size - bytes_recd, 1024))
			if chunk == b'':
				self.logMessage("TRANSMISSION","Failed to retrieve data from server when expected")
				return -4
			chunks.append(chunk)
			bytes_recd = bytes_recd + len(chunk)
			
		# Assemble to plaintext
		resp = self.decode((b"".join(chunks)))
		
		# Tell the server that we received the data
		sock.sendall(self.encode(self.config["accepted"]))
		
		# Close the socket and pass back the response
		sock.close()
		return resp