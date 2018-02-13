#! /usr/bin/env python3

"""
    Librium Initialization
"""

__author__ = 'Josh Bosley'

import os
import sys
import time
from demiurge import Demiurge
from librium import Librium

class MyDemiurge(Demiurge):
    def run(self):
        self.librium = Librium()
        while True:
            # Run scan
            self.librium.run_object_scan()
            time.sleep(10)


def build_item():
    units = ['k', 'm', 'g', 't', 'p', 'e']
    location = input("Path of item to be capped : ")
    ans = input("File, or Directory ?  (f/d) : ").lower()
    if 'f' == ans:
        if os.path.isfile(location):
            type = "file"
        else:
            print("{} , the FILE does not exist.")
            return
    else:
        if os.path.isdir(location):
            type = "dir"
        else:
            print("{} , the DIR does not exist.")
            return

    # Print keys
    for a, b, c in zip(units[::3], units[1::3], units[2::3]):
        print('{:<30}{:<30}{:<}'.format(a, b, c))

    which = input("What unit should the cap me measured in ?  : ")
    if which not in units:
        print("Invalid Option")
        return

    cap = input("What do you want to set the cap to? (int) : ")
    try:
        cap = int(cap)
    except ValueError:
        print("Non-Int entered.")
        return

    actions = ["clean", "notify", "remove", "notify-clean", "notify-remove", ""]
    for a, b, c in zip(actions[::3], actions[1::3], actions[2::3]):
        print('{:<30}{:<30}{:<}'.format(a, b, c))
    action = input("Which action would you like to run? : ")
    if action not in actions:
        print("Invalid Option")
        return

    new_item = ('{"location":"' + location +
                '", "type":"' + type +
                '","unit":"' + which +
                '","max":' + str(cap) +
                ',"action":"' + action +
                '"}')
    return new_item

def handle_add():
    """
            Helps the user make a new Librium item.
    """

    new_item = build_item()
    temp = Librium()
    temp.add_item(new_item)
    del temp
    print("Added new item : \n   {}".format(new_item))
    return

def handle_edit():
    """
            Helps the user change an existing Librium item.
    """

    h = 0
    item_dict = {}
    temp = Librium()
    current_objects = temp.get_items()
    for i in current_objects:
        item_dict[h] = i["location"]
        print("{} : {}".format(h,item_dict[h]))
        h += 1

    user_selection = input("Please select item [0 - {}]: ".format((h-1)))

    try:
        user_selection = int(user_selection)
    except ValueError:
        print("User selection not intiger value.. Exiting")
        return
    if user_selection < 0 or user_selection > h:
        print("User selection out of range.. Exiting")
        return

    print("User selected : {}".format(user_selection))
    temp.update_item(user_selection, build_item())
    del temp
    return

def main():

    if len(sys.argv) != 2:
        print('Usage: {} [summon|banish|reset|add]'.format(sys.argv[0]))
        return

    demiurge = MyDemiurge('Librium-Demiurge')

    if 'summon' == sys.argv[1]:
        demiurge.summon()
    elif 'banish' == sys.argv[1]:
        demiurge.banish()
    elif 'reset' == sys.argv[1]:
        demiurge.reset()
    elif 'add' == sys.argv[1]:
        handle_add()
        demiurge.reset()
    elif 'edit' == sys.argv[1]:
        handle_edit()
        demiurge.reset()
    else:
        sys.exit(2)
    sys.exit(0)

if __name__ == '__main__':
    main()
