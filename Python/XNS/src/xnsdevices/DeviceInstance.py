import sys
import time
import threading

from xnscommon import settings

class DeviceInstance(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.signal = True
        self.uid = "_UNSET_"
        self.device_name = "_UNSET_"
        self.device_slots = {}
        self.device_signals = {}

    '''     Add a device slot (Something that recvs info)   '''
    def addDeviceSlot(self, slotName, slotMethod):
        try:
            self.device_slots[slotName] = slotMethod
        except KeyError:
            print("Error: Duplicate device slot : ", slotName)
            exit(1)

    '''     Add a device signal (To trigger an action)      '''
    def addDeviceSignal(self, signalName, signalMethod):
        try:
            self.device_signals[signalName] = signalMethod
        except KeyError:
            print("Error: Duplicate device signal : ", signalName)
            exit(1)
	
    '''     The method the device instance needs to override '''
    def mainLoop(self):
        print("Override this method to create a device routine!")
	
    def run(self):
        while self.signal:
            self.mainLoop()
            time.sleep(1)








