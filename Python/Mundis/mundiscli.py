
import gherkindb
import mundisuser
import connector
import getpass

__location__ = '/etc/mundis-server/'
__config__ = '/etc/mundis-server/config.db'
__schemas__ = '/etc/mundis-server/schemas.db'

class MundisCli:
    def __init__(self):
        self.db = gherkindb.load(__config__, True)
        self.config = self.db.get('config')
        self.server_token = self.db.get('server-encryption-token')
        self.cout = self.db.sget('cout')
        self.prompt = "mcli$"
        self.schemadb = gherkindb.load(__schemas__, True)
        self.schema_list = list(self.schemadb.getall())
        self.user_manager = mundisuser.management()

        logged_in = False
        try:
            while not logged_in:
                print("Please enter the Mundis root password to continue:")
                rpass = getpass.getpass(prompt="Password: ")
                try:
                    logged_in = self.user_manager._verifyUser("root", rpass, self.server_token, "users.db", 9000)
                except mundisuser.ManagementException:
                    pass
        except KeyboardInterrupt:
            exit(0)

        try:
            self.con = connector.munconnect(
                self.config["address"], 
                self.config["port"], 
                "root", rpass, 'users.db'
                )
            self.con.connect()
        except:
            print("Unable to connect")
            exit(0)

        self.commandLoop()

    def commandLoop(self):
        self.cout("blue", "Starting Mundis Command Line Interface. Use ^C to exit")
        try:
            while True:
                self.cout("cyan", self.prompt, "")
                self.user_in = input()
                self.parseInput()
        except KeyboardInterrupt:
            exit(0)

    def parseInput(self):
        chunks = self.user_in.split(" ")

        if self.user_in == "help":
            print('''

    lsusers     List all users
    lsdb        List all databases

    create user 'username'
    create db 'database'
    modify user 'username'
    modify db 'database'

            ''')
            return

        if self.user_in == "lsusers":
            ulist = list(self.user_manager.userdb.getall())
            count = 0
            self.cout("yellow", "Current users:")
            print("\t", end="")
            for user in ulist:
                count += 1
                if 0 == count % 10:
                    print(user, " ")
                    print("\n", end="")
                else:
                    print(user, " ", end="")
            print()
            return
        
        if self.user_in == "lsdb":
            dblist = self.schema_list
            count = 0
            self.cout("yellow", "Databases:")
            print("\t", end="")
            for dbs in dblist:
                count += 1
                if 0 == count % 10:
                    print(dbs)
                    print("\n", end="")
                else:
                    print(dbs, " ", end="")
            print()
            return

        if len(chunks) == 0:
            return

        if len(chunks) < 3:
            self.cout("fail", "Unknown command")
            return

        if chunks[0] == "create":
            if chunks[1] == "user":
                self.createUser(chunks[2])
            elif chunks[1] == "db":
                self.dbCreate(chunks[2])
            else:
                self.cout("fail", "Unknown command")

        elif chunks[0] == "modify":
            if chunks[1] == "user":
                self.modifyUser(chunks[2])
            elif chunks[1] == "db":
                self.dbModify(chunks[2])
            else:
                self.cout("fail", "Unknown command")

    '''
        Create a new user
    '''
    def createUser(self, user):
        print("Create user: ", user)
        try:
            print("Creating", user)
            upass = getpass.getpass(prompt="Please enter a password for the user: ")
            selection = "_"
            perms = {
                "0": "GET",
                "1": "SET",
                "2": "ALL"
            }
            permtrans = {
                "0": 4000,
                "1": 5000,
                "2": 9000
            }

            # Select database
            print("Currently available databases: ")
            print(self.schema_list)
            dbselection = "::::"
            while dbselection not in self.schema_list:
                dbselection = input("Database selection: ")

            # Grant permission
            print("Which permissions would you like to give the user on the db:")
            for x in perms:
                print(" ", x, ":", perms[x])
            print("")
            while selection not in perms:
                selection = input(": ")
            print("Giving the user", user, "permissions to", perms[selection], "on", dbselection)

            try:
                self.user_manager._newUser(user, upass, [
                    {
                        "database": dbselection,
                        "permissions": [permtrans[selection]]
                    }
                ])
            except mundisuser.ManagementException as e:
                self.cout("fail", e)

            # Call on the connection to refresh user list
            self.con.refresh()

        except KeyboardInterrupt:
            self.cout("fail", "\nCancelling user creation")
            return

        return

    '''
        Modify a user

    '''
    def modifyUser(self, user):
        print("Modify user: ", user)
        return

    def dbCreate(self, db):
        db = db.replace(" ", "_")
        self.cout("yellow", "Creating database "+db)
        location = "/var/lib/mundis-server/"+db

        self.cout("yellow", "Adding definition to db schema")
        tdb = gherkindb.load(__schemas__, True)
        tdb.set(db, location)

        self.cout("yellow", "Creating the database in /var/lib/mundis-server/")
        ndb = gherkindb.load(location, True)

        self.schemadb = gherkindb.load(__schemas__, True)
        self.schema_list = list(self.schemadb.getall())

    def dbModify(self, db):
        if db not in self.schema_list:
            self.cout("fail", "No database matching " + db)
            self.cout("blue", "Use lsdb to list databases")

        print("NYD")
