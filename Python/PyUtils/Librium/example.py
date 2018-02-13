#!/usr/bin/env python3

from librium import Librium

__author__ = 'Josh Bosley'

def main():

    # Initialize Librium
    l = Librium()

    print("""

                    Initiating object scan.

    """)

    # Check sized ( Every 10 Sec or so )
    l.run_object_scan()

    print("""

                    Object scan complete.

    """)

    # Show existing Items
    l.display_objects()

    # Add Item to config list
    l.add_item('{"location":"/tmp/example_dir", "type":"dir","unit":"m","max":2,"action":"clean"}')

if __name__ == '__main__':
    main()
