import threading
import SocketServer
import request_map as rmap

# Settings, and allowed transactions
server_settings = {
        "BLOCK": 1024,
        "DELIM": "^",
        "UKRES": "?",
        "HOST": "10.1.0.77",
        "PORT": 5004
}
recognized_queries = ['n', 'u', 'r']

# Primary server handling
class TCPConnectionHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        reply = None
        self.data = self.request.recv(server_settings["BLOCK"]).split(server_settings["DELIM"])
        if self.data[0] not in recognized_queries:
                print "Unknown request was given to the server"
                reply = server_settings["UKRES"]
        else:
                print "The server got a valid request"
                reply = rmap.handle(self.data)
        if reply is not None:
                self.request.send(reply)
        self.request.close()

# The server 
class Server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    daemon_threads = True
    allow_reuse_address = True
    def __init__(self, server_address, RequestHandlerClass):
        SocketServer.TCPServer.__init__(self, server_address, RequestHandlerClass)

# Initiation instance for threading
def initiate_dcs():
    the_server = Server((server_settings["HOST"], server_settings["PORT"]), TCPConnectionHandler)
    the_server.serve_forever()
    return

# If this is the entry point, lets just start the server
if __name__ == '__main__':
    print "Starting DCS Server"
    threads = []
    the_server_thread = threading.Thread(target=initiate_dcs)
    threads.append(the_server_thread)
    the_server_thread.start()
    print "DCS server is active"
