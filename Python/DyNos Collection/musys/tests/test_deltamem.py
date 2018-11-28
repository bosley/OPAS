
# NAM - Network attatched memory
from musys import netmem

# NAMC - Network attatched memory channel accessor
from musys import memchan

import threading

port = 9001
udi = "node-test-"
auth = "Some_auth_token_x112358x256"

NAM = netmem(auth, 9001)
NAM.daemon = True
NAM.start()

class dmemtest(threading.Thread):

    def __init__(self,num):
        threading.Thread.__init__(self)
        self.num = str(num)
        self.new_chan = memchan(auth, port, udi+self.num)

    def run(self):
        for i in range(1,10):
            self.run_NAMC()

    def run_NAMC(self):

        string_data = "some string data that needs to be saved"
        self.new_chan.put("data_label", string_data)
        result = self.new_chan.fetch("data_label")

        print(result)
        assert result == string_data

        int_data = int(self.num)
        self.new_chan.put("data_label-1", int_data)
        result = self.new_chan.fetch("data_label-1")

        print(result)
        assert result == int_data

        def some_func():
            print("Success")

        func_data = some_func
        self.new_chan.put("data_label-2", some_func)
        result = self.new_chan.fetch("data_label-2")

        try:
            result()
        except:
            print("Failure fetching function. THREAD:", self.num)
            raise Exception("Data integrity lost")

        result = self.new_chan.fetch("something_that_doesnt_exist")

        print(result)
        assert result == None

pool = []
for i in range(1,4):
    n = dmemtest(i)
    pool.append(n)

print("moot")
[x.start() for x in pool]

