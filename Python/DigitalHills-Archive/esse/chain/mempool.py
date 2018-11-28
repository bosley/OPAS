'''
    Copyright 2017 Digital Hills, LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
'''

from queue import Queue
from chain import typeTrans
from chain import transaction
from settings import block_settings
from json import loads, dumps

'''
    Subclass of queue for thread-safe copying 
    of queued objects
'''
class PoolQueue(Queue):
    def request_for_block(self):
        with self.mutex:
            return list(self.queue)

    def request_for_insert(self, txData):
        self.put(txData)

'''
    The memory pool of pending transactions on local node
'''
class MemPool:
    def __init__(self):
        self.pending = []
        self.memQueue = PoolQueue()

    '''
        Add a transaction into the memory pool's queue
    '''
    def insert_transaction(self, txData):
        existing = self.memQueue.request_for_block()
        ctx = loads(txData)
        for x in existing:
            if x["id"] == ctx["id"]:
                return "_TRANSACTION_ALREADY_EXISTS_"
        self.memQueue.request_for_insert(ctx)
        return "_NEW_TX_CREATED_"

    '''
        Request transactions to create block
    '''
    def request_for_block(self, txNum):
        if txNum < block_settings["_MINIMUM_TXS_"]:
            return (False, "Request must be larger than minimum tx")
        temp = self.memQueue.request_for_block()
        if len(temp) < block_settings["_MINIMUM_TXS_"]:
            return (False, "Not enough tx in queue to generate block")
        if len(temp) < txNum:
            return (False, "Requested number of tx not in queue")
        return (True, dumps(temp))

    '''
        External call for size of memory pool
    '''
    def request_pool_size(self):
        return(True, str(len(self.memQueue.request_for_block())))
        



    