from .tcpout import query
from .channel import channel

class _ip_connection:
    def __init__(self, connection_info):
        self.channel = channel()
        self.connection_info = connection_info
        self.valid_tcp_response = lambda x: True if (x != -1 and x != -3 and x is not None) else False

    def setup(self):
        self.remote_address = self.connection_info[0]
        self.port = self.connection_info[1]
        return True

    '''
        Triggered by connector to send data
    '''
    def output(self, data):
        # Try to send the data 5 times. If we fail to send 5 times, 
        # place a FAILED_TO_SEND in the received queue
        success = False
        for x in range(0, 5):
            response = query(self.remote_address, self.port, data)
            if self.valid_tcp_response(response):
                self.channel.received.put(response)
                success = True
                break
        if not success:
            self.channel.received.put("FAILED_TO_SEND")
            
