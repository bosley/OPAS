
from datetime import datetime
from json import load, dump, dumps

from core import *
from sysfunc import *

from display import cout, ctxt

class Journal:
    def __init__(self):
        self.data = None
        self.journal_path = DataPath + "journal.json"
        try:
            with open(self.journal_path) as r:
                self.data = load(r)
        except IOError:
            self.data = {
                "init": "[CREATED->"+ EnsureST(datetime.now()) + "]" 
                }
            self.save()

    '''
        Save the contents of the journal
    '''
    def save(self):
        cout("cyan", "...saving...")
        try:
            with open(self.journal_path, 'w+') as w:
                dump(self.data, w)
        except IOError:
            alert_dialog_n_response("Error","Unable to write journal file...")

    '''
        Entry point for call
    '''
    def entryCall(self, arguments):
        returnInfo = return_info("Journal")
        returnInfo.addMessage("Entry Called")

        self.current_working_file = None
        self.active = True
        while self.active:
            x = input(ctxt("cyan", "journal") + ctxt("red", ">"))
            if x == 'q' or x == 'quit':
                self.save()
                self.active = False
                returnInfo.addMessage("User Exit")
                return returnInfo

            if x == 'help' or x == 'h':
                cout("yellow", '''
                
    view ........ Show entire file
    tail ........ Show last lines of file
    set ......... Set current working file
    add ......... Append to current working file
    what ........ Tell what file is set
    clear ....... Clear screen
    quit, q ..... Quit journal

                ''')
        
            if x == 'what':
                if self.current_working_file is not None :
                    cout("yellow", self.current_working_file)
                else:
                    cout("yellow", "No file set")

            if x == 'clear':
                ClearScreen()

            if x == 'save':
                self.save()

            pieces = x.split(" ")

            if len(pieces) > 0:
                if len(pieces) < 2:
                    cout("yellow", "Usage:  <command> <log_entry> ")
                else:
                    if pieces[0] == 'set':
                        if pieces[1] == 'init':
                            cout("yellow", "Not allowed - use a different name")
                        else:
                            self.current_working_file = pieces[1]
                            if pieces[1] not in self.data:
                                self.data[self.current_working_file] = []

                    if pieces[0] == 'view':
                        if pieces[1] not in self.data:
                            cout("yellow", "Unknown Log")
                        else:
                            ClearScreen()
                            for x in self.data[self.current_working_file]:
                                displayFixedWidthText("lightgreen", x)

                    if pieces[0] == 'tail':
                        if self.current_working_file is not None :
                            for x in self.data[self.current_working_file][:5]:
                                displayFixedWidthText("lightgreen", x)
                        else:
                            cout("yellow", "No file set - Use 'set <log>'")
                            
                    if pieces[0] == 'add':
                        if len(pieces) < 3:
                            cout("yellow", "usage <add> <log> <some data>")
                        else:
                            if self.current_working_file is not None :
                                self.data[self.current_working_file].append(' '.join(pieces[2:]))
                            else:
                                cout("yellow", "No file set - Use 'set <log>'")
                                
                            
                            


        
        returnInfo.addMessage("Entry Full Exit")
        return returnInfo