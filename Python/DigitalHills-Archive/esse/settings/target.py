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

'''
        Target calculator
'''

_INITIAL_TARGET_ = 1
_WEEK_IN_SEC_ = 604800
_MINIMUM_TX_PER_BLOCK_ = 10
_BLOCKS_BETWEEN_ADJUST_ = 1000

class target:
    
    def __init__(self, network):
        # Operating network
        self.network = network.split(".")[0]

        # Beginning target
        self.cTarget = _INITIAL_TARGET_

        # How many blocks between adjustments
        self.nBlockAdjust = _BLOCKS_BETWEEN_ADJUST_

        # Time-frame (sec) to release nBlockAdjust k
        self.timeFrame = _WEEK_IN_SEC_

    def adjust_difficulty(self, actualReleaseTime):
        if self.network == "dev":
            self.cTarget = 1
            return

        # Adjust the target if we aren't on the dev network (testnet)
        self.cTarget =(self.cTarget * (self.timeFrame/actualReleaseTime))
        if self.cTarget < 1:
            self.cTarget = 1

    def get_minimum_transactions(self):
        return _MINIMUM_TX_PER_BLOCK_
    
    def get_difficulty(self):
        return self.cTarget

'''
    Test the target adjustment
'''
if __name__ == '__main__':

    # Create a target, and tell it its on the test network so 
    # diff will be adjusted
    targetAdjust = target("test.a001")
    print("Initial difficulty: ", targetAdjust.cTarget)

    targetAdjust.adjust_difficulty(604800/2)
    print("Took half the time as expected: ", targetAdjust.cTarget)

    targetAdjust.adjust_difficulty(604800*4)
    print("Took 4 times as expected: ", targetAdjust.cTarget)

    targetAdjust.adjust_difficulty(604800/3)
    print("Took 1/3 the time that was expected: ", targetAdjust.cTarget)
