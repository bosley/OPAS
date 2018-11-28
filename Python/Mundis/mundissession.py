
import threading
from uuid import uuid4
from time import sleep, time

'''
    Session Manager

    A utility gauge user sessions interacting
    with the databases
'''
class SessionManager(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.lock = threading.Lock()
        self.active_sessions = {}
        self.active = True
        self.prune = []

    '''
        Create a new session, and send back the session id
    '''
    def generateSession(self, ttl):
        if not self.active:
            return None
        new_session_id = str(uuid4())
        with self.lock:
            self.active_sessions[new_session_id] = (time(), ttl)
        return new_session_id

    '''
        End a session by given id
    '''
    def endSession(self, session_id):
        if session_id in self.active_sessions:
            with self.lock:
                del self.active_sessions[session_id]

    '''
        Check to see if a session is still active
    '''
    def checkSession(self, session_id):
        if not self.active:
            return False
        if session_id in self.active_sessions:
            return True
        return False

    '''
        Kill all sessions, and stop handing out
        new sessions
    '''
    def killAllSessions(self):
        self.active = False
        with self.lock:
            self.active_sessions.clear()

    '''
        Run as a thread until killAllSessions is 
        triggered, or self.active is remotely set
        to False
    '''
    def run(self):
        while self.active:
            ctime = time()
            for session in self.active_sessions:
                if ctime - self.active_sessions[session][0] > self.active_sessions[session][1]:
                    self.prune.append(session)
            for old_session in self.prune:
                with self.lock:
                    del self.active_sessions[old_session]
            self.prune.clear()
            sleep(.2)

if __name__ == '__main__':

    import random
    print("Starting session test")

    session_manager = SessionManager()
    session_manager.daemon = True
    session_manager.start()

    new_session = session_manager.generateSession(3500)

    for x in range(1, 2000):
        session_manager.generateSession(random.randint(2,60))

    input("\nBLOCK\n")

    if session_manager.checkSession(new_session):
        print("Our session is active")
    else:
        print("Our session was not found")

    input("\nBLOCK\n")

    session_manager.endSession(new_session)

    input("\nLAST BLOCK\n")