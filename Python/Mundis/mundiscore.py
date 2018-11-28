
import gherkindb
import mundisserve
from time import sleep
from demiurge import Demiurge

__location__ = '/etc/mundis-server/'
__config__ = '/etc/mundis-server/config.db'

class Muncore(Demiurge):

    ''' Start the server '''
    def run(self):
        self._loadsettings()

        try:
            self.mserver = mundisserve.Munserve()
            self.mserver.daemon = True
            self.mserver.start()
        except MServerException as e:
            self.info_out("ERROR", e)
            exit(1)

        self.running = True
        while self.running:
            sleep(1)

    ''' Streamlined message out '''
    def info_out(level, message):
        self.cout(output[level], message)

    ''' Load configuration information from the database '''
    def _loadsettings(self):
        self.db = gherkindb.load(__config__, True)
        self.config = self.db.get('config')
        self.cout = self.db.sget('cout')

        # Standardized colored output
        self.output = {
            "ERROR": "fail",
            "WARN": "yellow",
            "OKAY": "lightgreen",
            "INFO": "cyan"
        }

