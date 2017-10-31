
from core import *

class Senses:
    def __init__(self):
        print("Senses class..")
        pass

    '''
        Entry point for call
    '''
    def entryCall(self, arguments):
        returnInfo = return_info("Senses")
        returnInfo.addMessage("Entry Called")

        
        returnInfo.addMessage("Entry Full Exit")
        return returnInfo