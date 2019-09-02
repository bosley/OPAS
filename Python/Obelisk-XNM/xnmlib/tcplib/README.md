# TCP Lib 
## Digital Hills, LLC

This is a simple multi-threaded server thread instance that enables
multiple threaded server threads. 

### Server usage example: 

> from time import sleep
> from tcplib import tcpServerInstance
> 
> '''
>	Start the server
> '''
>
> my_serv = tcpServerInstance("127.0.0.1", 9000)
> my_serv.daemon = True
> my_serv.signal = True
> my_serv.start()
>
> try:
> 	while True:
>		print("Do something in your main thread while the server slaves away!")
>	sleep(5)
>
> except KeyboardInterrupt:
>
> 	'''
> 		Shutdown the server
> 	'''
> 	my_serv.signal = False
> 	my_serv.join()
> 	my_serv.daemon = False


### Transmitting usage example

> import logging
> from transmitter import query
>
> logging.basicConfig(format='%(asctime)s [TRANSMITTER]: %(message)s <%(levelname)s>')
> thelogger = logging.getLogger(__name__)
> t = "*" * 2 ** 8
> query("127.0.0.1", 9000, t, thelogger)
>





