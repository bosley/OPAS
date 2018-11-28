
import sqlite3
from json import loads as jloads
from json import dumps as jdumps
from datetime import datetime

class DataStore:

    def __init__(self):
        self.pillar_db = 'xmn_node.db'
        self.node_db = 'xmn_node.db'
        self.ensure_db(self.pillar_db)
        self.ensure_db(self.node_db)
        self.ensureStr = lambda x: x if isinstance(x, str) else str(x)
        return

    def get_db(self, store):
        try:
            db = sqlite3.connect(store, timeout=15)
        except:
            print("Unable to open the data store : ", store)
            exit(1)
        return db

    def ensure_db(self, wdb):
        db = self.get_db(wdb)
        try:
            db.execute('''
                CREATE table pillars (
                    ip TEXT NOT NULL,
                    id TEXT NOT NULL,
                    port TEXT NOT NULL,
                    last_online TEXT NOT NULL
                )
            ''')
            print("> Pillar table created")
        except:
            pass

        try:
            db.execute('''
                CREATE table nodes (
                    uid TEXT,
                    ip TEXT,
                    last_pull TEXT,
                    last_connect TEXT,
                    port TEXT NOT NULL
                )
            ''')
            print("> Node table created")
        except:
            pass

        try:
            db.execute('''
                CREATE table messages (
                    fro TEXT,
                    gto TEXT,
                    creation TEXT,
                    previous_hash TEXT,
                    hash TEXT,
                    status TEXT,
                    message TEXT
                )
            ''')
            print("> Messages table created")
        except:
            pass

        try:
            db.execute('''
                CREATE table keys (
                    id TEXT,
                    public TEXT,
                    private TEXT
                )
            ''')
            print("> Keys table created")
        except:
            pass

        db.close()
        return

    def remove_keys(self, accessor, id):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        c = db.cursor()
        c.execute("DELETE FROM tokens WHERE id = '{s}'".format(s=id))
        db.commit()
        db.close()

    def remove_pillar(self, accessor, id):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        c = db.cursor()
        c.execute("DELETE FROM pillars WHERE id = '{s}'".format(s=id))
        db.commit()
        db.close()

    def remove_node(self, accessor, id):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        c = db.cursor()
        c.execute("DELETE FROM nodes WHERE uid = '{s}'".format(s=id))
        db.commit()
        db.close()

    def remove_message(self, accessor, hash):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        hash = self.ensureStr(hash)
        c = db.cursor()
        c.execute("DELETE FROM messages WHERE hash = '{s}'".format(s=hash))
        db.commit()
        db.close()

    def add_pillar_info(self, accessor, ip, id, port, last_online):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        
        res = self.get_pillar(accessor, id)
        if len(res) != 0:
            return (True, "exists")
            
        db = self.get_db(access)
        ip = self.ensureStr(ip)
        id = self.ensureStr(id)
        port = self.ensureStr(port)
        last_online = self.ensureStr(last_online)
        stmt = '''INSERT INTO pillars (ip, id, port, last_online) VALUES ("{a}","{b}","{c}", "{d}");'''.format(a=ip, b=id,c=port, d=last_online)
        db.execute(stmt)
        db.commit()
        db.close()
        return (True, "success")

    def add_node_info(self, accessor, uid, ip, lp, lc, port):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db

        res = self.get_node(accessor, uid)
        if len(res) != 0:
            return (True, "exists")
            
        db = self.get_db(access)
        uid = self.ensureStr(uid)
        ip = self.ensureStr(ip)
        lp = self.ensureStr(lp)
        lc = self.ensureStr(lc)
        port = self.ensureStr(port)
        stmt = '''INSERT INTO nodes (uid, ip, last_pull, last_connect, port) VALUES ("{a}","{b}","{c}","{d}","{e}");'''.format(a=uid, b=ip, c=lp, d=lc, e=port)
        db.execute(stmt)
        db.commit()
        db.close()
        return (True, "success")

    def add_message_info(self, accessor, fro, to, creation, phash, hash, status, msg):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        fro = self.ensureStr(fro)
        to = self.ensureStr(to)
        creation = self.ensureStr(creation)
        phash = self.ensureStr(phash)
        hash = self.ensureStr(hash)
        status = self.ensureStr(status)
        msg = self.ensureStr(msg)
        stmt = '''INSERT INTO messages 
        (fro, gto, creation, previous_hash, hash, status, message) 
        VALUES 
        ("{a}","{b}","{c}","{d}","{e}","{f}","{g}");
        '''.format(a=fro, b=to, c=creation, d=phash, e=hash, f=status, g=msg)
        db.execute(stmt)
        db.commit()
        db.close()
        return (True, "success")

    def add_keys_info(self, accessor, id, pub, priv):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db

        res = self.get_keys(accessor, id)
        if len(res) != 0:
            return (True, "exists")
            
        db = self.get_db(access)
        id = self.ensureStr(id)
        pub = self.ensureStr(pub)
        priv = self.ensureStr(priv)
        stmt = '''INSERT INTO keys
        (id, public, private) VALUES 
        ("{a}", "{b}", "{c}");
        '''.format(a=id, b=pub, c=priv)
        db.execute(stmt)
        db.commit()
        db.close()
        return (True, "success")

    def get_messages_by_status(self, accessor, owner, status):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        owner = self.ensureStr(owner)
        status = self.ensureStr(status)
        c = db.cursor()
        stmt = '''SELECT * FROM messages 
        WHERE status = '{s}' AND gto = '{t}';
        '''.format(s=status, t=owner)
        c.execute(stmt)
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "fro":x[0], 
                "gto":x[1], 
                "creation":x[2], 
                "previous_hash":x[3],
                "hash":x[4],
                "status":x[5],
                "message":x[6]
                }) for x in data]

        # Reading the messages out of the db marks them as read
        [self.set_message_read(accessor, x["hash"]) for x in members]
        # Send members
        return members

    def get_messages_all_unread(self, accessor):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        c = db.cursor()
        stmt = '''SELECT * FROM messages 
        WHERE status = 'unread';'''
        c.execute(stmt)
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "fro":x[0], 
                "gto":x[1], 
                "creation":x[2], 
                "previous_hash":x[3],
                "hash":x[4],
                "status":x[5],
                "message":x[6]
                }) for x in data]

        # Send members
        return members
        
    def get_messages_all_read(self, accessor):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        c = db.cursor()
        stmt = '''SELECT * FROM messages 
        WHERE status = 'read';'''
        c.execute(stmt)
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "fro":x[0], 
                "gto":x[1], 
                "creation":x[2], 
                "previous_hash":x[3],
                "hash":x[4],
                "status":x[5],
                "message":x[6]
                }) for x in data]

        # Send members
        return members

    def set_message_read(self, accessor, hash):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        hash = self.ensureStr(hash)
        stmt = "UPDATE messages SET status = 'read' WHERE hash = '{s}';".format(s=hash)
        db.execute(stmt)
        db.commit()
        db.close()

    def get_pillars(self, accessor):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        c = db.cursor()
        c.execute("SELECT * FROM pillars;")
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "ip":x[0], 
                "id":x[1],
                "port":x[2],
                "last_online": x[3]
                }) for x in data]
        return members

    def get_nodes(self, accessor):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        c = db.cursor()
        c.execute("SELECT * FROM nodes;")
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "uid":x[0], 
                "ip":x[1],
                "last_pull":x[2],
                "last_connect":x[3],
                "port": x[4]
                }) for x in data]
        return members

    def get_node(self, accessor, uid):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        uid = self.ensureStr(uid)
        db = self.get_db(access)
        c = db.cursor()
        c.execute("SELECT * FROM nodes WHERE uid = '{a}';".format(a=uid))
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "uid":x[0], 
                "ip":x[1],
                "last_pull":x[2],
                "last_connect":x[3],
                "port": x[4]
                }) for x in data]
        return members

    def get_pillar(self, accessor, uid):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        uid = self.ensureStr(uid)
        db = self.get_db(access)
        c = db.cursor()
        c.execute("SELECT * FROM pillars WHERE id = '{a}';".format(a=uid))
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "ip":x[0],
                "id":x[1],
                "port":x[2],
                "last_online": x[3]
                }) for x in data]
        return members

    def get_keys(self, accessor, id):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        c = db.cursor()
        c.execute("SELECT * FROM keys WHERE id = '{a}';".format(a=id))
        data = c.fetchall()

        if len(data) == 0:
            return data

        members = []
        [members.append(
            {
                "id":x[0],
                "public":x[1],
                "private":x[2]
                }) for x in data]
        return members

    def update_node_activity(self, accessor,id, ip, lp, lc):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        ip = self.ensureStr(ip)
        lp = self.ensureStr(lp)
        lc = self.ensureStr(lc)
        stmt = '''UPDATE nodes
                SET ip = "{a}", 
                last_pull = "{b}",
                last_connect = "{c}"
                WHERE uid = "{d}";
            '''.format(a=ip, b=lp, c=lc, d=id)
        db.execute(stmt)
        db.commit()
        db.close()

    def update_pillar_activity(self, accessor, ip, id, lo):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        ip = self.ensureStr(ip)
        lo = self.ensureStr(lo)
        stmt = '''UPDATE pillars
                SET ip = "{a}",
                last_online = "{b}"
                WHERE id = "{c}";
            '''.format(a=ip, b=lo, c=id)
        db.execute(stmt)
        db.commit()
        db.close()

    def update_keys(self, accessor, id, pub, priv):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        pub = self.ensureStr(pub)
        priv = self.ensureStr(priv)
        stmt = '''UPDATE keys SET public = "{a}", private = "{b}" WHERE id = "{c}";'''.format(a=pub, b=priv, c=id)
        db.execute(stmt)
        db.commit()
        db.close()

    def delete_message_by_hash(self, accessor, hash):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        hash = self.ensureStr(hash)
        stmt = '''DELETE FROM messages WHERE hash = '{a}'; '''.format(a=hash)
        db.execute(stmt)
        db.commit()

    def delete_node_by_uid(self, accessor, uid):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        uid = self.ensureStr(uid)
        stmt = '''DELETE FROM nodes WHERE uid = '{a}'; '''.format(a=uid)
        db.execute(stmt)
        db.commit()

    def delete_pillar_by_id(self, accessor, id):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        stmt = '''DELETE FROM pillars WHERE id = '{a}'; '''.format(a=id)
        db.execute(stmt)
        db.commit()

    def delete_key_by_id(self, accessor, uid):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        uid = self.ensureStr(uid)
        stmt = '''DELETE FROM keys WHERE id = '{a}'; '''.format(a=uid)
        db.execute(stmt)
        db.commit()

    def update_pillar_connect(self, accessor, id, last_connect):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        db = self.get_db(access)
        id = self.ensureStr(id)
        stmt = '''UPDATE pillars SET last_online = "{a}" WHERE id = "{b}";'''.format(a=last_connect, b=id)
        db.execute(stmt)
        db.commit()
        db.close()

    def update_node_online(self, accessor, id, last_online):
        if accessor != "node" and accessor != "pillar":
            return(False, "Error: Accessor must be node or pillar")
        if accessor == "node":
            access = self.node_db
        else:
            access = self.pillar_db
        id = self.ensureStr(id)
        db = self.get_db(access)
        stmt = '''UPDATE nodes SET last_connect = "{a}" WHERE uid = "{b}";'''.format(a=last_online, b=id)
        db.execute(stmt)
        db.commit()
        db.close()
