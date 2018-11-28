import dill
import gherkindb


'''
    Database OBJ specific exception
'''
class MDataObjectException(Exception):
    def __init__(self, dErrArguments):
        Exception.__init__(self, "Error: {0} | {1}".format(
            dErrArguments[0], dErrArguments[1]
            ))
        self.dErrArguments = dErrArguments


'''
    User-defined database object
'''
class DatabaseObject:
    def __init__(self, name, location):
        self.name = name
        self.location = location
        self.datastore = gherkindb.load(self.location, True)
        self.router = {
            "get": self.get,
            "getli": self.getli,
            "getdv": self.getdv,
            "set": self.set,
            "setli": self.setli,
            "setdv": self.setdv
        }
        self.permission_list = {
            "SET": 4000,
            "GET": 5000,
            "ALL": 9000
        }

    '''
        Standardize response for connector
    '''
    def _generateResponse(self, title, report, data):
        return {
            "title": title,
            "report": report,
            "data": dill.dumps(data)
        }

    '''
        Check user rights
    '''
    def _checkUserPermissions(self, callClass, permissions):
        if permissions == self.permission_list["ALL"]:
            return True
        if permissions == self.permission_list[callClass]:
            return True
        return False 
        
    '''
        Standard permission issue
    '''
    def _returnPermissionIssue(self):
        return _generateResponse(
            "ERROR", 
            "INSUFFICIENT PERMISSIONS", 
            "User lacks required permissions"
        )

    '''
        GET some data from the database


    '''
    def get(self, request):
        if not self._checkUserPermissions("GET", request[1]):
            return self._returnPermissionIssue()

        get_value = None
        query = request[0]
        in_keys = query.keys()

        # Get all keys from DB
        if len(in_keys) == 0:
            return self._generateResponse(
                "OKAY",
                "QUERY SUCCESS",
                list(self.datastore.getall())
            )

        # Get value by key
        if "key" in in_keys:
            try:
                get_value = self.datastore.get(query["key"])
            except KeyError:
                return self._generateResponse(
                    "ERROR",
                    "INVALID KEY",
                    False
                )
            return self._generateResponse(
                "OKAY",
                "QUERY SUCCESS",
                get_value
            )

        # Get by type
        if "type" in in_keys:
            
            # Find all keys with value type specified
            type_list = []
            all_keys = self.datastore.getall()
            for local_key in all_keys:
                if type(self.datastore.get(local_key)) == query["type"]:
                    type_list.append(local_key)
            return self._generateResponse(
                "OKAY",
                "QUERY SUCCESS",
                type_list
            )
    
    ''' 
        Get list index 


    '''
    def getli(self, request):
        if not self._checkUserPermissions("GET", request[1]):
            return self._returnPermissionIssue()

        query = request[0]
        index_value = None
        try:
            index_value = self.datastore.lget(query["key"], query["index"])
        except KeyError:
            return self._generateResponse(
            "ERROR",
            "INVALID KEY",
            False
            )
        except TypeError:
            return self._generateResponse(
            "ERROR",
            "NON LIST ITEM",
            False
            )
        except IndexError:
            return self._generateResponse(
            "ERROR",
            "INVALID LIST INDEX",
            False
            )
        return self._generateResponse(
            "OKAY",
            "QUERY SUCCESS",
            index_value
        )
    
    ''' 
        Get dict value 
    '''
    def getdv(self, request):
        if not self._checkUserPermissions("GET", request[1]):
            return self._returnPermissionIssue()

        get_value = None
        query = request[0]
        try:
            get_value = self.datastore.dget(query["key"], query["value"])
            return self._generateResponse(
            "ERROR",
            "INVALID KEY",
            False
            )
        except TypeError:
            return self._generateResponse(
            "ERROR",
            "NON DICT ITEM",
            False
            )
        return self._generateResponse(
            "OKAY",
            "QUERY SUCCESS",
            get_value
        )
    
    '''
        SET some data in the database


    '''
    def set(self, request):
        if not self._checkUserPermissions("SET", request[1]):
            return self._returnPermissionIssue()

        query = request[0]
        in_keys = query.keys()

        # Standard set key to value
        if "key" in in_keys:
            self.datastore.set(
                query["key"], query["value"]
            )
            return self._generateResponse(
                "OKAY",
                "QUERY SUCCESS",
                True
            )

    ''' 
        Set list index 
    '''
    def setli(self, request):
        if not self._checkUserPermissions("SET", request[1]):
            return self._returnPermissionIssue()

        query = request[0]
        in_keys = query.keys()
        
        operating_list = self.datastore.get(query["key"])

        # If there is no item, create a new list
        if operating_list is None:
            operating_list = []
        elif not isinstance(operating_list, list):
            return self._generateResponse(
            "ERROR",
            "NON SET LIST CAN NOT OPERATE ON NON-LIST TIEM",
            True
            )
        operating_len = len(operating_list)-1

        # No sub-zero appends
        if query["index"] < 0:
            return self._generateResponse(
                "ERROR",
                "INDEX MUST BE GREATER THAN ZERO",
                True
            )

        # If the index exceeds the list size, overflow must be set to continue
        if query["index"] > operating_len:
            if "overflow" in in_keys:
                if query["overflow"] == "append":
                    operating_list.append(query["value"])
                elif query["overflow"] == "expand":
                    while len(operating_list)-1 != query["index"]:
                        operating_list.append(None)
                    operating_list[query["index"]] = query["value"]
            else:
                return self._generateResponse(
                    "ERROR",
                    "INDEX EXCEEDS LIST LENGTH WITH NO SET OVERFLOW",
                    True
                )
        else:
            operating_list[query["index"]] = query["value"]

        self.datastore.set(query["key"], operating_list)

        return self._generateResponse(
            "OKAY",
            "QUERY SUCCESS",
            True
        )
    
    ''' 
        Set dict value 
    '''
    def setdv(self, request):

        query = request[0]

        if not self._checkUserPermissions("SET", request[1]):
            return self._returnPermissionIssue()

        if not isinstance(query["value"], tuple):
            return self._generateResponse(
            "ERROR",
            "NON-TUPLE VALUE IN setdv",
            False
            )


        try:
            self.datastore.dadd(query["key"], query["value"])
        except IndexError:
            return self._generateResponse(
            "ERROR",
            "POTENTIAL NON-TUPLE VALUE IN setdv",
            False
            )
        except KeyError:
            return self._generateResponse(
            "ERROR",
            "INVALID KEY",
            False
            )
        return self._generateResponse(
            "OKAY",
            "QUERY SUCCESS",
            True
        )
