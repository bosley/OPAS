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

from json import dumps
from datetime import datetime

from dhash import mine
from chain import block
from dhash import hasher
from settings import block_settings

'''
    BLOCKCHAIN
'''
class blockchain:
    def __init__(self):
        self.head = 0
        self.chain = []

    def mineGenesis(self):
        if self.head != 0 or len(self.chain) != 0:
            return (False, "Chain already in operation, can't re-mine genesis!")
        print("Mine the genesis!")

    '''
        Add Block
            Take-in a mined block, and verify it. If it is a valid block, 
            it is added into the chain
    '''
    def addBlock(self, block):
        # Ensure correct parameters were set in block
        if block.nonce is None:
            return (False, "Invalid nOnce - Denied")
        if len(block.txs) < block_settings["_MINIMUM_TXS_"]:
            return (False, "Block does not have minimum number of transactions - Denied")
        if block.time is None:
            return (False, "Block time not set - Denied")
        if block.previousBlock is not None:
            return (False, "Previous block was set - Denied")
        if block.blockHash is None:
            return(False, "No hash for block given - Denied")

        # Validate the transaction hashes
        for txHash, txData in block.txs.items():
            ctxHash = hasher(block_settings["_TX_SEED_"], txData)
            if ctxHash != txHash:
                return(False, "Invalid transaction: {}".format(txHash) )

        # Determine what the previous hash should be
        if len(self.chain) == 0:
            block.previousBlock = "_ABYSS_"
        else:
            block.previousBlock = self.getBlockByIndex(self.head)[1].blockHash

        # Ensure that the block's hash is valid
        if not block.blockHash.startswith('0'*block_settings["_DIFFICULTY_"]):
            return (False, "Invalid block hash")

        # Recalculate the hash to double-check
        if block.blockHash != hasher(block.nonce, dumps(block.txs, sort_keys=True)[0]):
            return (False, "Invalid block hash")
        
        self.chain.append(block)
        self.head = len(self.chain) - 1
        return (True, "Block Added")

    def getBlockByIndex(self, _index):
        if _index < len(self.chain):
            return (True, self.chain[_index])
        else:
            return (False, "Index out of chain bounds")

    def getBlockByTx(self, _tx):
        for b in self.chain:
            if _tx in b.txs:
                return (True, b)
        return (False, "TX not found")

    def getBlockIndexByTx(self, _tx):
        index = 0
        for b in self.chain:
            if _tx in b.txs:
                return (True, index)
            index += 1
        return (False, "No block found by tx")

    def getBlockIndexByHash(self, _hash):
        index = 0
        for b in self.chain:
            if b.blockHash == _hash:
                return (True, index)
            index += 1
        return (False, "Unable to get index by hash")

    def showBlocks(self):
        for block in self.chain:
            print(block)
    
    def getHead(self):
        return self.head

    def verifyChain(self):
        print("Verify Chain")

        pHash = "_ABYSS_"
        index = 0
        for block in self.chain:
            
            '''
                Make sure that the transactions are in dictionary type
            '''
            if not isinstance(block.txs, dict):
                return(False, "Transaction type error")
            
            '''
                Validate each transaction in current block
            '''
            for txHash, txData in block.txs.items():
                ctxHash = hasher(block_settings["_TX_SEED_"], txData)
                if ctxHash != txHash:
                    return(False, "Invalid transaction: {}".format(txHash) )
                
            '''
                Validate each prev and current block hash 
            '''
            bHash = hasher(block.nonce, dumps(block.txs, sort_keys=True)[0])

            if block.previousBlock != pHash:
                print("Previous block: ", block.previousBlock, ". Expected: ", pHash, "Index: ", index)
                return (False, "Previous block error")
            if block.blockHash != bHash:
                print("Block hash: ", block.blockHash, ". Expected: ", bHash, "Index: ", index)
                return (False, "Current block error")
            pHash = bHash
        return (True, "Valid")
