
from core import *

class Comms:
    def __init__(self):
        print("Comms class..")
        pass

    '''
        Entry point for call
    '''
    def entryCall(self, arguments):
        returnInfo = return_info("Comms")
        returnInfo.addMessage("Entry Called")

        
        returnInfo.addMessage("Entry Full Exit")
        return returnInfo