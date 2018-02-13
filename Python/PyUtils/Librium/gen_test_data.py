"""

            Generate data for testing purposes

"""


__author__ = 'Josh Bosley'

import os

config = '/var/librium/config'
test = '/home/jbosley/pytest/test'

open(config, 'w').close()

if not os.path.isdir('/home/jbosley/pytest'):
    os.mkdir('/home/jbosley/pytest')

if not os.path.isdir('/var/librium'):
    os.mkdir('/var/librium')

for i in range(1,5):

    # Create test dirs
    item = (test + str(i))
    if not os.path.isdir(item):
        os.mkdir(item)

    # Create test documents
    for j in range(1, 11):
        sub_file = (item + "/item" + str(j))
        if not os.path.isfile(sub_file):
            f = open(sub_file, 'w')
            f.write("test\n")
            f.close()

    # Insert items into config file
    try:
        # Try to write out the default settings
        f = open(config, 'a')
        default = '{"location":"' + item + '", "type":"file","unit":"k","max":20,"action":"clean"}\n'
        f.write(default)
        f.close()
    except OSError:
        print("Unable to create {}".format(config))
        raise