import os
import json
import notify
from shutil import rmtree

__author__ = 'Josh Bosley'

class Librium:

    def __init__(self):
        """
                Setup Librium, load files if exist, if not create
        """

        # Setup filenames
        self.location = '/var/librium'
        self.config = self.location + '/config'
        self.log = '/var/log/Librium-Demiurge.log'

        self.type_file = "file"
        self.type_dir = "dir"

        # Map actions to methods
        self.actions = {
            "clean": self.clean_object,
            "notify": self.send_notification,
            "remove": self.remove_object,
            "notify-clean": self.notify_clean,
            "notify-remove": self.notify_remove
        }

        # Supported size units (Kilobytes, megabytes, etc)
        self.units = {'k' : 1, 'm': 2, 'g' : 3, 't' : 4, 'p' : 5, 'e' : 6 }

        if not os.path.isdir(self.location):
            try:
                os.mkdir(self.location)
            except OSError:
                print('Unable to create {}'.format(self.location))
                raise

        # Create list to store objects from self.config, and load them
        self.object_list = []
        self.load_settings()

        # A place to store mutliple return values for a call
        self.return_values = {}
        # Place to store what methods failed
        self.incomplete_methods = ""

    def load_settings(self):
        """
                Loads JSON data from self.conifg
        """

        # If the log file doesnt exist
        if not os.path.isfile(self.config):
            print("No config file found, attempting to create")
            try:
                # Try to write out the default settings
                f = open(self.config, 'w+')
                default = '{"location":"' + self.log + '", "type":"file","unit":"m","max":10,"action":"clean"}\n'
                f.write(default)
                f.close()
            except OSError:
                print("Unable to create {}".format(self.config))
                raise

        # Open up config file and load each object
        with open(self.config, 'r') as f:
            for line in f.readlines():
                obj = json.loads(line)
                self.object_list.append(obj)

        # Load Notifications
        self.notify = notify.Notify()

    def convert_bytes(self, bytes, to, block=1024):
        """
                Converts the bytes given to the specified unit
        """
        result = float(bytes)
        for r in range(self.units[to]):
            result = result / block
        return result

    def run_object_scan(self):
        """
                Go through each item from config file and see if they have reached
                their cap or not. If they have, perform their action
        """

        number_of_objects_updated = 0

        for obj in self.object_list:
            # Retrieve object allowed size, ensure it is int, represents MB
            try:
                # Ensure that cap is stored as int
                object_cap = int(obj["max"])
            except ValueError:
                print("Invalid cap configuration for {}.".format(obj["location"]))
                raise

            # See if file or dir to correctly calculate size
            if obj["type"] == self.type_file:
                # Retrieve the object's size
                try:
                    object_size = os.stat(obj["location"])
                    object_size = float(object_size.st_size)

                except OSError:
                    print("Could not find item {}  . . . Skipping ".format(obj["location"]))
                    continue

            elif obj["type"] == self.type_dir:
                object_size = 0

                if os.path.isdir(obj["location"]):
                    for dirpath, dirnames, filenames in os.walk(obj["location"]):
                        for f in filenames:
                            fp = os.path.join(dirpath, f)
                            object_size += os.path.getsize(fp)
                else:
                    print("Could not find item {}  . . . Skipping ".format(obj["location"]))
                    continue

            else:
                print("Invalid type in item {}".format(obj["location"]))
                continue

            # Convert from bytes to specified cap unit
            object_size = self.convert_bytes(object_size, obj["unit"])

            # If the current size is > than the allowe size
            if object_size > object_cap:
                # Get index of current object in settings, and send off to have its action perfomed
                if 1 == self.perform_object_action(self.object_list.index(obj)):
                    number_of_objects_updated += 1
                    print("Item {} action performed".format(obj["location"]))
                else:
                    print("Failed to perform an action on item {}".format(obj["location"]))

            # Reset error handlers
            self.incomplete_methods = ""
            self.return_values = {}

        print("[ {} ] items updated".format(str(number_of_objects_updated)))
        return

    def perform_object_action(self, object_index):
        """
                Takes in index location of item being watched and performs its action
                if it has a valid action

                return 1 if completed
                return -1 if error

                called by run_object_scan
        """

        return_value = 1
        obj = self.object_list[object_index]

        if obj["action"] not in self.actions.keys():
            return_value = -1
            print("Unknown action in {}".format(obj["location"]))
            return return_value

        # Call the action that needs to be performed
        self.actions[obj["action"]](object_index)

        # Check if action(s) were completed
        for key in self.return_values.keys():
            vals = self.return_values[key]
            if len(vals) > 0:
                if vals[0] != 1:
                    return_value = -1
                    self.incomplete_methods += ("[" + key + "]")

        if len(self.incomplete_methods) > 1:
            print("ERROR: {}".format(self.incomplete_methods))

        return return_value

    """

            Available Actions

    """

    def send_notification(self, object_index):

        message = []
        self.return_values["notify"] = []
        obj = self.object_list[object_index]

        # Build message data by file, or dir
        if obj["type"] == self.type_file:
            try:
                data = []
                with open(obj["location"], 'w') as f:
                    message = f.readlines()
            except OSError:
                self.return_values["notify"].append(-1)
                self.return_values["notify"].append("Could not read file {}".format(obj["location"]))

        elif obj["type"] == self.type_dir:
            message.append("DIRECTORY LISTING\n")
            for the_file in os.listdir(obj["location"]):
                file_path = os.path.join(obj["location"], the_file)
                message.append((file_path + "\n"))
        else:
            self.return_values["notify"].append(-1)
            return

        # Send the message
        self.notify.by_email(''.join(message))

    def clean_object(self, object_index):
        """
                Take in index of item in settings that needs to be cleaned, folder or file
                This method does not delete the folder or the file, but cleans it out
        """

        self.return_values["clean"] = []
        print("cleaning object")

        obj = self.object_list[object_index]

        # Handle Files
        if obj["type"] == self.type_file:
            try:
                open(obj["location"], 'w').close()
            except OSError:
                self.return_values["clean"].append(-1)
                self.return_values["clean"].append("Could not clean file {}".format(obj["location"]))

        elif obj["type"] == self.type_dir:
            for the_file in os.listdir(obj["location"]):
                file_path = os.path.join(obj["location"], the_file)
                try:
                    if os.path.isfile(file_path):
                        os.unlink(file_path)
                    elif os.path.isdir(file_path):
                        rmtree(file_path)
                except OSError:
                    self.return_values["clean"].append(-1)
                    self.return_values["clean"].append("Could not clean dir {}".format(obj["location"]))
        else:
            self.return_values["clean"].append(-1)
            self.return_values["clean"].append("Unknown type for {}".format(obj["location"]))

    def remove_object(self, object_index):
        """
                     Delete the folder, or file
        """

        obj = self.object_list[object_index]

        # Handle Files
        if obj["type"] == self.type_file:
            try:
                os.remove(obj["location"])
            except OSError:
                self.return_values["remove"].append(-1)
                self.return_values["remove"].append("Could not remove file {}".format(obj["location"]))

        elif obj["type"] == self.type_dir:
            try:
                rmtree(obj["location"])
            except OSError:
                self.return_values["remove"].append(-1)
                self.return_values["remove"].append("Could not clean dir {}".format(obj["location"]))
        else:
            self.return_values["remove"].append(-1)
            self.return_values["remove"].append("Unknown type for {}".format(obj["location"]))
        return

    def notify_clean(self, object_index):
        """
                Perform notification, then clean
        """
        self.send_notification(object_index)
        self.clean_object(object_index)
        return

    def notify_remove(self, object_index):
        """
                Perform notification, then removal
        """
        self.send_notification(object_index)
        self.remove_object(object_index)
        return

    def display_objects(self):
        """
                Pretty print all objects
        """
        for obj in self.object_list:
            print(json.dumps(obj, sort_keys=True, indent=4))
        return

    def get_items(self):
        """
                Send out the current objects
        """
        return self.object_list

    def add_item(self, json_string):
        """
                Adds new items to be scanned. Will not allow
                items to be added if path, and type are the same
        """
        jObj = json.loads(json_string)
        for obj in self.object_list:
            if obj["location"] == jObj["location"]:
                if obj["type"] == jObj["type"]:
                    print("Item already exists. Not added.")
                    return -1

        # If it got here, its new. Add it
        s = json_string + "\n"
        with open(self.config, "a") as f:
            f.write(s)

        # Reload settings to include new item
        self.object_list = []
        self.load_settings()
        return

    def update_item(self, object_index, new_json_data):
        """
                Update an object with new data
        """
        self.object_list[object_index] = json.loads(new_json_data)
        f = open(self.config, 'w')
        for obj in self.object_list:
            f.write(json.dumps(obj) + "\n")
        f.close()

        print("Updating item {} , with data {}".format(object_index, new_json_data))
        return
