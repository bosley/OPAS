
import sl4a
from os import system
from datetime import datetime
from display import cout

droid = sl4a.Android()

RootPath='/sdcard/qpython/projects3/BRAIN/'
DataPath=RootPath+'data_store/'

EnsureDT = lambda x: x if isinstance(x, datetime) else datetime.strptime(x, '%Y-%m-%d %H:%M:%S')
EnsureST = lambda x: x if isinstance(x, str) else '{0:%Y-%m-%d %H:%M:%S}'.format(x)
ClearScreen = lambda : system("clear")

MaxTextWidthDisplay = 45

# Display text in a fixed-width manner
def displayFixedWidthText(color, line):
    if len(line) > MaxTextWidthDisplay:
        lines = [line[i:i+MaxTextWidthDisplay] for i in range(0, len(line), MaxTextWidthDisplay)]
        for x in lines:
            cout(color, x)
    else:
        cout(color,line)

# Structured return information
class return_info:
    def __init__(self, info):
        self.info = info
        self.messages = []
    def addMessage(self, message):
        self.messages.append(message)