
import gherkindb
from uuid import uuid4
from hashlib import sha256
from base64 import b64decode, b64encode
from xmlrpc.server import SimpleXMLRPCServer

__location__ = '/etc/mundis-server/'
__userdb__ = '/etc/mundis-server/users.db'
__config__ = '/etc/mundis-server/config.db'


''' Custom exception raised by mgmt class '''
class ManagementException(Exception):
    pass

'''
    MGMT is for user management and control.
    Creating, deleting, and validating users.
    This class is not 
'''
class management:
    def __init__(self):
        self.userdb = gherkindb.load(__userdb__, True)

    def refreshDatabase(self):
        del self.userdb
        self.userdb = gherkindb.load(__userdb__, True)

    '''
        Create a new user
    '''
    def _newUser(self, user, password, access_params):

        # Ensure type
        if not isinstance(access_params, list):
            raise ManagementException("Error: Invalid access parameter type")
            return False

        # Ensure the user isn't a duplicate
        if user in self.userdb.getall():
            raise ManagementException("Error: User exists")
            return False

        # Ensure correct access parameter definitions
        for entry in access_params:
            if not isinstance(entry, dict):
                raise ManagementException("Error: Invalid type in access parameter entry")
                return False
            else:
                if "database" not in entry or "permissions" not in entry:
                    raise ManagementException("Error: Invalid construction of parameter entry")
                    return False
                if not isinstance(entry["database"], str):
                    raise ManagementException("Error: Invalid database entry ")
                    return False
                if not isinstance(entry["permissions"], list):
                    raise ManagementException("Error: Invalid permissions entry type")
                    return False
                for perm in entry["permissions"]:
                    if not isinstance(perm, int):
                        raise ManagementException("Error: Invalid permission entry")
                        return False

        # Create salt, and password hash. Add to mem-type dict
        user_salt = uuid4().hex
        self.userdb.set(user, 
        {
            "salt": user_salt,
            "password": self._hashPassword(password, user_salt),
            "access": access_params
        })

    '''
        Verify a username and password, given the user-group
    '''
    def _verifyUser(self, user, password, database, permission_request):

        # Ensure user exists
        if user not in self.userdb.getall():
            raise ManagementException("Error: Unknown User")
            return False

        # Validate user password
        user_data = self.userdb.get(user)
        if user_data["password"] != self._hashPassword(password, user_data["salt"]):
            raise ManagementException("Error: Invalid Password")
            return False

        # Validate permission (database, requested action)
        if not isinstance(permission_request, int):
            raise ManagementException("Error: Invalid permission_request type")
            return False

        user_access_defined = user_data["access"]
        for entry in user_access_defined:
            if database == entry["database"]:
                if permission_request in entry["permissions"]:
                    return True
                else:
                    return False
        return False

    '''
        Generate a hash based on the text, and salt of a user
    '''
    def _hashPassword(self, plainText, salt):
        hasher_maw = sha256()
        hasher_maw.update(plainText.encode('utf-8'))
        hasher_maw.update(salt.encode('utf-8'))
        return hasher_maw.hexdigest().encode('utf-8')

    '''
        Remove a user from the database
    '''
    def _deleteUser(self, user, password, memory_access):

        # Ensure user exists
        if user not in self.userdb.getall():
            raise ManagementException("Error: Unknown User")
            return False

        # Delete the user
        self.userdb.rem(user)
        return True

'''
    Built-In test to check the functionality of mgmt
'''
if __name__ == '__main__':
    print("Testing umgmt")
    mngr = management()  

    permissions = {
        "SET": 1,
        "GET": 2,
    }

    print("Creating a new user with SELECT permissions on test-db")
    try:
        mngr._newUser("test", "some_test_password", [
            {
                "database": "test-db",
                "permissions": [permissions["SELECT"]]
            }
        ])
    except ManagementException as e:
        print(e)

    print("Testing valid action")
    valid = False
    try:
        valid = mngr._verifyUser("test", "some_test_password", "test-db", permissions["SELECT"])
    except ManagementException as e:
        print(e)
    if valid:
        print("Valid action ", permissions["SELECT"], " for user test")
    else:
        print("Invalid action! - This shoudnt be seen")

    print("Testing invalid action")
    valid = False
    try:
        valid = mngr._verifyUser("test", "some_test_password", "test-db", permissions["UPDATE"])
    except ManagementException as e:
        print(e)
    if valid:
        print("Valid action ", permissions["SELECT"], " for user test")
    else:
        print("Invalid action!")

    print("Testing invalid database")
    valid = False
    try:
        valid = mngr._verifyUser("test", "some_test_password", "mooter", permissions["UPDATE"])
    except ManagementException as e:
        print(e)
    if valid:
        print("Valid action ", permissions["SELECT"], " for user test")
    else:
        print("Invalid action!")

    print("Testing invalid password")
    valid = False
    try:
        valid = mngr._verifyUser("test", "SOME_INVALID_PASS", "test-db", permissions["SELECT"])
    except ManagementException as e:
        print(e)
    if valid:
        print("Something went wrong!")
