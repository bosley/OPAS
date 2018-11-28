
import dill
import logging
import xmlrpc.client

import mundisquery
import mundiscrypto

class MunConnectError(Exception):
    pass

class munconnect:
    def __init__(self, server, port, server_token, username, password, database, user_permissions="ALL"):
        try:
            self.port = int(port)
        except ValueError as _err:
            raise MunConnectError("Invalid port")
        if self.port < 1 or self.port > 65535:
            logging.error("Port number out of range")
            raise MunConnectError("Invalid port")

        mcrypto = mundiscrypto.MundisCrypto(server_token)

        self.log_response = False

        '''
            User information
        '''
        self.permission_request = {
            "SET": 4000,
            "GET": 5000,
            "ALL": 9000
        }
        self.username = username
        self.password = mcrypto.encode(password)
        self.database = database
        self.session = None
        try:
            self.permissions = self.permission_request[user_permissions]
        except KeyError:
            raise MunConnectError("Invalid user permission given [SET|GET|ALL]")

        '''
            Query verification
        '''
        self.validator = mundisquery.Validator()

        '''
            Server proxy
        '''
        self.server = server
        self.port = str(self.port)
        self.host = "http://"+self.server+":"+self.port
        self.accessor = xmlrpc.client.ServerProxy(self.host)

    '''
        Filter back data dict from server
    '''
    def _filterReturnData(self, data):
        if self.log_response:
            if "ERROR" == data["title"]:
                logging.warning("Error: "+ str(data["report"]))
            else:
                logging.warning("Success")
        return data["data"]

    '''
        Connect to a server, creating a session
    '''
    def connect(self):
        if self.session is not None:
            raise MunConnectError("User-defined session already active")

        self.session = self._filterReturnData(
            self.accessor.newSession(
                self.username,
                self.password,
                self.database,
                self.permissions
            )
        )

    '''
        Disconnect from a server, killing the session
    '''
    def disconnect(self):
        self._filterReturnData(
            self.accessor.killSession(self.session)
            )
    
    def refresh(self):
        self.accessor.performRefresh(self.session)

    '''
        Call the validator to ensure the query is in a valid format
    '''
    def ensureValid(self, query_type, query):
        validated_query = None
        try:
            validated_query = self.validator.router[query_type](query)
        except mundisquery.MundisQueryException as e:
            logging.error(e)
        return validated_query

    '''
        Package and send the query to the database
    '''
    def performQuery(self, type, user_query):
        validated_query = self.ensureValid(type, user_query)
        if validated_query is None:
            return False
        validated_query = dill.dumps(validated_query)

        returned = self._filterReturnData(
            self.accessor.query({
                "session": self.session,
                "type": type,
                "query": validated_query
            }))

        try:
            return dill.loads(returned.data)
        except AttributeError:
            return [returned]

    '''
        Get and set commands
    '''
    def get(self, user_query):
        return self.performQuery("get", user_query)

    def getli(self, user_query):
        return self.performQuery("getli", user_query)

    def getdv(self, user_query):
        return self.performQuery("getdv", user_query)

    def set(self, user_query):
        return self.performQuery("set", user_query)

    def setli(self, user_query):
        return self.performQuery("setli", user_query)

    def setdv(self, user_query):
        return self.performQuery("setdv", user_query)

    
if __name__ == '__main__':

    import gherkindb
    config_db = gherkindb.load('/etc/mundis-server/config.db', False)
    server_token = config_db.get('server-encryption-token')

    con = munconnect("127.0.0.1", 9001, server_token ,"test", "a_basic_password", 'test_db')
    con.log_response = True
    con.connect()

    result = con.set({
        "key": "my_dict",
        "value": {
            "key1": "val1",
            "key2": "val2"
        }
    })

    for key in con.get({}):
        print(key, ":", con.get({
            "key": key
        }))

