
import sqlite3
from json import loads as jloads
from json import dumps as jdumps
from datetime import datetime

class DataStore:

    def __init__(self):
        self.xnsdb = 'xnsdb/xns.db'
        self.ensure_db()
        return

    def get_db(self, store):
        try:
            db = sqlite3.connect(store, timeout=15)
        except:
            print("Unable to open the data store : ", store)
            exit(1)
        return db

    def ensure_db(self):
        db = self.get_db(self.xnsdb)
        try:
            db.execute('''
                CREATE table logs (
                    date TEXT NOT NULL,
                    module TEXT NOT NULL,
                    message TEXT NOT NULL
                )
            ''')
            print("> Log table created")
        except:
            pass

        try:
            db.execute('''
                CREATE table nodes (
                    uid TEXT,
                    name TEXT,
                    location TEXT,
                    address TEXT,
                    devices TEXT,
                    lastActivity TEXT,
                    nest TEXT
                )
            ''')
            print("> node table created")
        except:
            pass
        db.close()
        return

    def addLogEntry(self, log):
        
        if len(log) != 2:
            print("Error: Logging message. Invalid amount of information (module, message)")
            return False

        db = self.get_db(self.xnsdb)

        # Just to be sure strings
        log = [str(x) for x in log]

        stmt = '''INSERT INTO logs (date, module, message) VALUES ("{a}","{b}","{c}");'''.format(a=str(datetime.now()), b=log[0], c=log[1])
        db.execute(stmt)
        db.commit()
        db.close()
        return True

    # Expects a dictionary
    def addNodeActivity(self, d):
        db = self.get_db(self.xnsdb)
        c = db.cursor()
        c.execute("SELECT uid FROM nodes WHERE uid = '{s}'".format(s=d["UID"]))

        # If it doesn't exist, create it. If it does, update it
        if c.fetchone() is None:
            try:
                stmt = '''
                INSERT INTO nodes (uid, name, location, address, devices, lastActivity, nest) VALUES
                ("{a}", "{b}", "{c}", "{d}", "{e}", "{f}", "{g}");
                '''.format(
                    a=d["UID"],b=d["name"],c=d["location"],
                    d=d["address"],e=d["devices"],f=str(datetime.now()),g=d["nest_address"])
            except KeyError:
                print("CALL LOGGER HERE BECAUSE THERE WAS A KEY ERROR!")

                print("INSERT : ", d)
                return False
            db.execute(stmt)
            db.commit()
        else:
            try:
                stmt = '''
                UPDATE nodes 
                    SET uid = "{a}", name = "{b}", location = "{c}", 
                    address = "{d}", devices = "{e}", lastActivity = "{f}", nest = "{g}"
                WHERE uid = "{h}";
                '''.format(
                    a=d["UID"],b=d["name"],c=d["location"],
                    d=d["address"],e=d["devices"],f=str(datetime.now()),g=d["nest_address"],h=d["UID"])
            except KeyError:
                print("CALL LOGGER HERE BECAUSE THERE WAS A KEY ERROR!")

                print("UPDATE : ", d)
                return False
            db.execute(stmt)
            db.commit()
        # Wer're all done
        db.close()
        return True

    # Remove a node
    def removeNode(self, uid):
        db = self.get_db(self.xnsdb)
        c = db.cursor()
        c.execute("DELETE FROM nodes WHERE uid = '{s}'".format(s=uid))
        db.commit()
        db.close()

    # Returns all of the known nodes, and their activity 
    # in the form of a list of dictionary vars
    def getNodeActivity(self):
        db = self.get_db(self.xnsdb)
        c = db.cursor()
        c.execute("SELECT * FROM nodes;")
        data = c.fetchall()

        # If there is nothing there, return an empty list
        if len(data) == 0:
            return data

        # Make a list of dicts for the node info
        members = []
        [members.append(
            {
                "uid":x[0], 
                "name":x[1], 
                "location":x[2], 
                "address":x[3],
                "devices":x[4],
                "activity":x[5],
                "nest_address":x[6]
                }) for x in data]

        return members