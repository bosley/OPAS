#   Josh A. Bosley
#   The purpose of the connection class is to add a means of abstraction 
#   to the different types of connection. Making the connection ambiguous
#   will enable us to use the same syntax for connections despite them being
#   over wildly different means

from queue import Queue
from .ipConnection import _ip_connection
from .obeliskConnection import _obelisk_connection
# from bluetoothConnection import _bluetooth_connection
# from httpConnection import _http_connection
# from ipfsConnection import _ipfs_connection
# from usbConnection import _usb_connection
# from radioConnection import _radio_connection

class connection:
    def __init__(self, connection_type, receiver, port=None, identifier=None):
        self.ready = False
        if not isinstance(receiver, str):
            return
        if port is not None:
            if not isinstance(port, int):
                return 
        known_connections = {
            'obelisk': _obelisk_connection,
            'ip': _ip_connection
        }
        try:
            self.line = known_connections[connection_type]
        except KeyError:
            return

        self.line = self.line( (receiver, port, identifier) )
        self.ready = self.line.setup()
        self.outgoing_queue = Queue()

    def get_one(self):
        # Return one item from queue
        if self.line.channel.data_ready():
            return self.line.channel.get()

    def get_all(self):
        r = []
        while self.line.channel.data_ready():
            r.append(self.line.channel.get())
        return r

    def _signal_not_ready(self):
        return (False, "Line not ready")

    def send_one(self, data):
        if not self.ready:
            return self._signal_not_ready()
        return self.line.output(data)

    def send_add_to_queue(self, data):
        # Add message to outgoing queue, but don't execute
        if not self.ready:
            return self._signal_not_ready()
        self.outgoing_queue.put(data)

    def send_pour_queue(self):
        if not self.ready:
            return self._signal_not_ready()
        while not self.outgoing_queue.empty():
            self.send_one(self.outgoing_queue.get())
    