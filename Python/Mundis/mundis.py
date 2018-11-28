#! /usr/bin/env python3

import errno
import getpass
import argparse
import gherkindb
from shutil import rmtree
from os.path import exists
from os import makedirs, rmdir
import mundisuser
from demiurge import Demiurge
from mundiscore import Muncore
from mundiscli import MundisCli
from mundiscrypto import MundisCrypto, GenerateToken

__author__ = 'Josh Bosley'
__location__ = '/etc/mundis-server/'
__userdb__ = '/etc/mundis-server/users.db'
__config__ = '/etc/mundis-server/config.db'
__schemas__ = '/etc/mundis-server/schemas.db'
__datastore__ = '/var/lib/mundis-server/'

'''
    Colored output
'''
def cout(*args):
    color = {
        "blue": "\033[1;34m",
        "cyan": "\033[1;36m",
        "bold": "\033[;1m",
        "yellow": "\033[93m",
        "lightgreen": "\033[92m",
        "fail": "\033[91m"
    }
    if len(args) == 2:
        print(color[args[0]], args[1], "\033[0m")
    if len(args) == 3:
        print(color[args[0]], args[1], "\033[0m", end=args[2])

'''
    Create a schema
'''
def create_schema(schema_name, location):
    # Create test DB schema definition
    tdb = gherkindb.load(__schemas__, True)
    tdb.set(schema_name, location)

    # Create the DB
    ndb = gherkindb.load(location, True)
    ndb.set('__MUNDIS__ABOUT__', 'A default schema created on first-time setup.')
'''
    Run configuration
'''
def configure():
    db = gherkindb.load(__config__, True)

    address = port = None

    while address is None:
        temp = input('Address to bind server to: ')
        if input('Is ' + temp + ' correct? [y/n] ') == "y":
            address = temp

    while port is None:
        temp = input('Port to bind server to: ')
        if input('Is ' + temp + ' correct? [y/n] ') == "y":
            try:
                temp = int(temp)
            except ValueError:
                cout("fail", "Port must be type 'int'")
                continue
            if temp < 1 or temp > 65535:
                cout("fail", "Invalid port, given port is out of range")
                continue
            port = temp

    server_token = GenerateToken(32)
    db.set('server-encryption-token', server_token)

    print("New server token generated [", end="")
    cout("yellow", server_token, "")
    print("]")

    cout("cyan", ("Setting address to " + address + ", and port to " + str(port) ))
    db.set('config', {
        "address": address,
        "port": port
    })

    db.sset('cout', cout)

    # Create default 'test' schema
    create_schema('test_db', "/var/lib/mundis-server/test.db")

    # Create test user
    mngr = mundisuser.management()
    try:
        mngr._newUser("test", "a_basic_password", [
            {
                "database": "test_db",
                "permissions": [4000, 5000, 9000]
            }
        ])
    except ManagementException as e:
        print(e)


    rpass = 2
    rpass1 = -2
    while rpass != rpass1:
        cout("cyan", "Creating root user. Please enter a password for the root account.")
        rpass = getpass.getpass(prompt="Password:")
        cout("cyan", "Please re-enter the password")
        rpass1 = getpass.getpass(prompt="Password:")
        if rpass != rpass1:
            cout("fail", "Passwords don't match. Try again.")
    try:
        mngr._newUser("root", rpass, [
            {
                "database": "users.db",
                "permissions": [9000]
            }
        ])
    except ManagementException as e:
        print(e)

'''
    First time setup
'''
def first_run():
    try:
        makedirs(__location__)
        makedirs(__datastore__)
    except IOError as e:
        if e.errno == errno.EACCES:
            cout("fail", "Permission denied, need to be root.")
            exit(1)
    configure()

'''
    Entry point for the core system
'''
def cliEntry():

    '''
        Check if the file structure is setup - if not
        rnu first-time installation
    '''
    if not exists(__location__):
        cout("yellow", "First-time setup")
        try:
            first_run()
        except KeyboardInterrupt:
            cout("fail", "\nCancelling setup")
            rmdir(__location__)
            exit(1)
        cout("lightgreen", "First-time setup complete.")
        exit(1)

    '''
        Parse arguments from user
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', action='version', version='%(prog)s 0.0.1')
    parser.add_argument('--core', action="store", dest='core',
                        help='[start|stop|reset]')
    parser.add_argument('--setup', action="store_true", default=False, dest='setup',
                        help='Configure server settings')
    parser.add_argument('--lsconfig', action="store_true", default=False, dest='lsconfig',
                        help='Display server configuration')
    parser.add_argument('--uninstall', action="store_true", default=False, dest='uninstall',
                        help='Uninstall Server')
    parser.add_argument('--cli', action="store_true", default=False, dest='cli',
                        help='Server command line interface')

    results = parser.parse_args()

    '''
        Show server configuration
    '''
    if results.lsconfig:
        db = gherkindb.load(__config__, True)
        cout('yellow', db.get('config'))
        exit(0)

    '''
        User-initiated setup
    '''
    if results.setup:
        try:
            configure()
        except KeyboardInterrupt:
            cout("fail", "\nCancelling")
            exit(1)
        cout("lightgreen", "Configuration complete")
        exit(0)

    '''
        Check permissions before continuing
    '''
    try:
        with open(__location__+'/touch.temp', 'w') as o:
            pass 
    except IOError as e:
        if e.errno == errno.EACCES:
            cout("fail", "Permission denied, need to be root.")
            exit(1)

    '''
        Start the mundis command line interface
    '''
    if results.cli:
        MundisCli()

    '''
        Create the Muncore demiurge, and start, stop, or reset
    '''
    core_demiurge = Muncore("mundis-server")

    core_cmds = {
        "start": core_demiurge.summon,
        "stop": core_demiurge.banish,
        "reset": core_demiurge.reset
    }

    '''
        Uninstall server
    '''
    if results.uninstall:
        try:
            q = input("Are you sure you wan't to continue? [Y/n] ")
            if q != "Y":
                cout("fail", "Uninstall cancelled - 'Y' not detected")
                exit(1)
        except KeyboardInterrupt:
            cout("fail", "Uninstall cancelled - ^C detected")
            exit(1)
        
        cout('yellow', 'Attempting to stop any running Mundis Server')
        if exists('/tmp/mundis-server.pid'):
            core_cmds["stop"]()
            cout('lightgreen', "Mundis server stopped")
        
        try:
            rmtree(__location__)
            rmtree(__datastore__)
        except:
            cout('fail', "Uninstall failed")
        cout('lightgreen', "Uninstall successful")
        exit(0)
    
    '''
        User must use --core flag to proceed
    '''
    if results.core is None:
        cout("fail", "--core must be specified. Use -h to check usage.")
        exit(1)
    
    if results.core not in core_cmds:
        cout("fail", "Unknown command.")
        exit(1)
    
    try:
        core_cmds[results.core]()
    except RuntimeError as e:
        cout("fail", e)

if __name__ == '__main__':
    cliEntry()
