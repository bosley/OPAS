'''

        This is the testing router. It is meant to be a way to test simple
        functions that are being written in XNS. 

        router.route("testFunction", [arg1, ag2, arg3])

'''

from .columnPrinting import coltest

def route(args):
    
    testing = {
        "col": coltest,
    }

    testing[args[0]](args[1])
