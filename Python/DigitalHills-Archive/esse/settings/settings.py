'''
    Copyright 2017 Digital Hills, LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
'''

'''
        Exported settings
            Once the network starts running, these settings will be altered
            by the target class, with the exception of _TX_SEED_, which will
            be used to differenciate transactions on different implementations
            of the blockchain
'''

from sys import exit
from uuid import uuid4
from pathlib import Path
from json import dump, load
from .target import target

'''
    Node settings
'''
nodeFile = Path("settings/node.json")
node_settings = None
if nodeFile.is_file():
    with open("settings/node.json", 'r') as file:
        node_settings = load(file)
else:
    node_settings = {
        "nodeid": uuid4().hex,
        "local": "127.0.0.1",
        "port": 9000,
        "network": "dev.a001",
        "version": "0.0.1",
    }
    with open("settings/node.json", 'w+') as file:
        dump(node_settings, file)
if node_settings is None:
    print("...Fatal error generating settings file... [settings/settings.py]")
    exit(1)

'''
    Block settings
     - _DIFFICULTY_ and _MINIMUM_TXS_ used for testing.
'''
block_settings = {
    "_TX_SEED_": 314159,
    "_DIFFICULTY_": 1,
    "_MINIMUM_TXS_": 10
}

'''
    Targeting function
'''
targeting = target(node_settings["network"])

