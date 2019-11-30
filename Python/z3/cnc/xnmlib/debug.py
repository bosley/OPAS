
from .display import cout, ctxt

'''
    Output flags - To show processes underway
'''
__DISPLAY_TODO_OUTPUT__ = True
__DISPLAY_MERGE_OUTPUT__ = True
__DISPLAY_SCANNING_OUTPUT__ = False
__DISPLAY_SYNC_PILLAR_OUTPUT__ = True
__DISPLAY_SYNC_NODE_OUTPUT__ = True

def debug_TODO(location, text):
    cout("lightgreen", "[TODO]:" + location, "")
    cout("yellow", text)

def debug_VAROUT(location, variable_text):
    cout("blue", location, "")
    cout("yellow", variable_text)

def debug_MESSAGE(location, text):
    cout("cyan", location, "")
    cout("yellow", text)

def debug_GOOD(text):
    cout("lightgreen", text)

def debug_BAD(text):
    cout("fail", text)

def debug_FATAL(text):
    cout("red", text)