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

from display import ctxt, cout
from chain import blockchain
from chain import block
from dhash import mine
from dhash import hasher
from settings import block_settings

from json import dumps
from datetime import datetime

def _test_blockchain():

    # Make a chain!
    chain = blockchain()
    
    '''
            Create 10 blocks, with 20 tx each
    '''
    for x in range(0,10):
        t = block()
        t.txs = {}
        t.header = "BS_DEMO"
        print("Block #", x)
        for y in range(0,20):
            tx_data = "Transaction: " + str(y) + ", Time: " + str(datetime.now())
            txHash = hasher(block_settings["_TX_SEED_"], tx_data)
            t.txs[txHash] = (tx_data)

        # Mine the block
        t.nonce, t.blockHash = mine(dumps(t.txs, sort_keys=True)[0])

        # Attempt to add the block
        added, message = chain.addBlock(t)
        if added == False:
            print("Error adding block #", x, "> Reason: ", message)
            p = input(">>>")
        
    '''
            Get block by index number
    '''
    print("Block @ index 4:\n", chain.getBlockByIndex(4)[1])

    '''
            Get the block by a given transaction
    '''
    _gtx = input("Get block by tx > ")
    got, b = chain.getBlockByTx(_gtx)
    if not got:
        print("Unable to find any block with given tx")
    else:
        print(b)

    '''
            Get block index by given transaction
    '''
    _gtx = input("Get block index by tx > ")
    got, b = chain.getBlockIndexByTx(_gtx)
    if not got:
        print("Unable to find any block with given tx")
    else:
        print("Block index: ", b)

    '''
            Get block index by hash
    '''
    _ghsh = input("Get block index by block hash > ")
    got, b = chain.getBlockIndexByHash(_ghsh)
    if not got:
        print("Unable to find block by given hash")
    else:
        print("Block index: ", b)

    '''
            Show head index
    '''
    print("Head block : ", chain.getHead())

    '''
            Verify Chain
    '''
    validity, message = chain.verifyChain()
    if validity:
        print("Expected good, got: ", message)
    else:
        print("Expected good, got: ", message)
    
    # Break the chain, and re-verify
    chain.chain[3].blockHash = "00ss"

    validity, message = chain.verifyChain()
    if validity:
        print("Expected bad, got: ", message)
    else:
        print("Expected bad, got: ", message)

def fulltest():
    cout("cyan", "Testing blockchain functionality...")
    _test_blockchain()