
import dill
import gherkindb
import threading
import mundisdbobj
import mundiscrypto
import mundissession
import logging
from time import sleep, time
from xmlrpc.server import SimpleXMLRPCServer

import mundisuser

__config__ = '/etc/mundis-server/config.db'
__schemas__ = '/etc/mundis-server/schemas.db'
__userdb__ = '/etc/mundis-server/users.db'

__defaultSession__ = 86400

'''
    Server specific exception
'''
class MServerException(Exception):
    def __init__(self, dErrArguments):
        Exception.__init__(self, "Error: {0} | {1}".format(
            dErrArguments[0], dErrArguments[1]
            ))
        self.dErrArguments = dErrArguments

'''
    Mundis Server 
'''
class Munserve(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.lock = threading.Lock()
        self.errors = {
            "INVALID_USER_NAME": ("100", "Invalid user name"),
            "INVALID_USER_PASS": ("101", "Invalid user password"),
            "INVALID_USER_ACTION": ("102","Invalid user action")
        }

        self.user_refresh_cap = 10
        self.user_refresh = time()

        '''
            Users
        '''
        self.user_management = mundisuser.management()

        '''
            Databases

            location => dbobj
        '''
        self.schema_list = None
        self.database_objects = {}
        self._loadDataBases()

        '''
            Sessions manager
        '''
        self.session_manager = mundissession.SessionManager()
        self.session_manager.daemon = True
        self.session_manager.start()

        '''
            Tie a user session to the database they connected to
            sessionid => (database, permissions)
        ''' 
        self.session_to_database = {}

        '''
            Server config
        '''
        self.config_db = gherkindb.load(__config__, False)
        self.config = self.config_db.get("config")
        if self.config is None:
            raise MServerException((1000, "Server configuration not setup. Run installer"))

        self.mcrypt = mundiscrypto.MundisCrypto(
            self.config_db.get('server-encryption-token')
            )

        '''
            RPC Server
        '''
        self.server = SimpleXMLRPCServer(
            (self.config["address"],
             self.config["port"]),
             logRequests=False
             )
        self.server.register_function(self.newSession)
        self.server.register_function(self.killSession)
        self.server.register_function(self.performRefresh)
        self.server.register_function(self.query)

    '''
        Main run loop
    '''
    def run(self):
        self.server.serve_forever()

    '''
        Raise error with specific code
    '''
    def raiseError(self, friendly):
        raise MServerException(self.errors[friendly])

    '''
        Load all user-defined databases as dbobj class
        into self.database_objects
    '''
    def _loadDataBases(self):
        self.schema_list = gherkindb.load(__schemas__, True)
        for db in self.schema_list.getall():
            self.database_objects[db] = mundisdbobj.DatabaseObject(
                    db, self.schema_list.get(db)
            )

    '''
        Generate a standard response for the connector 
        to handle
    '''
    def _generateResponse(self, title, report, data):
        return {
            "title": title,
            "report": report,
            "data": data
        }

    ''' RPC available methods '''

    '''
        Create a new session for a user
        Ties the user session to their selected db, and their
        permissions
    '''
    def newSession(self, username, password, selected_database, permissions):

        print("ENC_PASS:", password)

        # Undo network encryption
        password = self.mcrypt.decode(password.data)
        print("PASSWORD: ", password)

        try:
            if not self.user_management._verifyUser(username, password, selected_database, permissions):
                logging.warning("Failed to authenticate user: " + username)
                return self._generateResponse("ERROR", "FAILED TO AUTHENTICATE USER", False)
        except mundisuser.ManagementException as e:
            return self._generateResponse("ERROR", "USER NOT FOUND", False)

        new_session = self.session_manager.generateSession(__defaultSession__)
        if new_session is None:
            logging.warning("Failed to generate new session for " + username)
            return self._generateResponse("ERROR", "FAILED TO GENERATE SESSION", False)

        self.session_to_database[new_session] = (selected_database, permissions)

        return self._generateResponse("OKAY", "SESSION GENERATED", new_session)

    '''
        Kills a user session
    '''
    def killSession(self, session_id):
        if self.session_manager.checkSession(session_id):
            self.session_manager.endSession(session_id)
        return self._generateResponse("OKAY", "SESSION KILLED", True)

    '''
        Triggered by remote to force-update user list
    '''
    def performRefresh(self, session_id):
        if not self.session_manager.checkSession(session_id):
            return self._generateResponse("ERROR", "INVALID SESSION", False)

        if time() - self.user_refresh >= self.user_refresh_cap:
            with self.lock:
                self.user_management.refreshDatabase()
                self._loadDataBases()
            self.user_refresh = time()

        return self._generateResponse("OKAY", "USERS REFRESHED", True)

    '''
        Takes in a user query, and passes it to the database
        that they are tied to along with their permissions
        on that database
    '''
    def query(self, query_data):

        # Peel data apart for readability
        session_id = query_data["session"]
        type = query_data["type"]
        user_query = dill.loads(query_data["query"].data)

        # Check the session 
        if not self.session_manager.checkSession(session_id):
            return self._generateResponse("ERROR", "INVALID SESSION", False)

        # Readability
        users_db = self.session_to_database[session_id][0]
        users_permissions = self.session_to_database[session_id][1]

        # Perform the query
        with self.lock:
            try:
                query_response = self.database_objects[users_db].router[type](
                    (user_query, users_permissions)
                    )
            except KeyError:
                return self._generateResponse("ERROR",
                "QUERY KEY ERROR", 
                False
                )
            except mundisdbobj.MDataObjectException as e:
                return self._generateResponse(
                    "ERROR", 
                    "Data Object Exception", 
                    e
                    )
        # All is well 
        return query_response


if __name__ == '__main__':

    try:
        test = Munserve()
        test.daemon = True
        test.start()
    except MServerException as e:
        print(e)
        exit(1)

    config = gherkindb.load(__config__, False)
    token = config.get('server-encryption-token')


    try:
        k = input("---- Press return to exit ----\n")
        test.signal = False
        test.join()
    except KeyboardInterrupt:
        pass
    