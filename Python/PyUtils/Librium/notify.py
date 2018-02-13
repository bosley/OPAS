__author__ = 'Josh Bosley'

import os
import json
import smtplib

class Notify:

    def __index__(self):

        self.email_list = []
        self.email_file = '/var/librium/email.config'
        self.load_settings()

    def load_settings(self):

        if not os.path.isfile(self.email_file):
            with open(self.email_file, 'w') as f:
                f.write('{"gmail-user": "username", "gmail-pass":"password","recpt":"recpt@site.com", "active":"no"}\n')

        with open(self.email_file, 'r') as f:
            for line in f.readlines():
                obj = json.loads(line)
                self.email_list.append(obj)

    def by_email(self, message):

        active_emails = []
        for item in self.email_list:
            if item["active"] == "yes":
                active_emails.append(item)

        if len(active_emails) < 1:
            print("Unable to notify by email. No account set in {}".format(self.email_file))
            return -1
        else:
            for active_email in active_emails:
                c = smtplib.SMTP('smtp.gmail.com',587)
                c.ehlo()
                c.starttls()
                c.ehlo()
                c.login(active_email["gmail-user"], active_email["gmail-pass"])
                header = ('To:'+active_email["recpt"]+'\n'+'From:'+"Librium-Notification"+'\n'+'Sub:Notify \n')
                msg = (header+'\n' + message +'\n\n')
                c.sendmail(active_email["gmail-user"], active_email["gmail-pass"], message)
                c.close()
        return