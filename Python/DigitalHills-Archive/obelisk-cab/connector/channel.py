from queue import Queue

class channel:
    def __init__(self):
        self.received = Queue()

    def data_ready(self):
        return (not (self.received.empty()))

    def insert(self, data):
        self.received.put(data)

    def get(self):
        if not self.received.empty():
            return self.received.get()
        else:
            return False
            
