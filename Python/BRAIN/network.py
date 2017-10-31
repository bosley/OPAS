
from core import *

class Network:
    def __init__(self):
        print("Network class..")
        pass

    '''
        Entry point for call
    '''
    def entryCall(self, arguments):
        returnInfo = return_info("Network")
        returnInfo.addMessage("Entry Called")

        
        returnInfo.addMessage("Entry Full Exit")
        return returnInfo