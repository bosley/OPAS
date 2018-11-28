from optparse import OptionParser

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

from node import DHNode

parser = OptionParser()
parser.add_option("-m", "--mode", dest="mode",
                  help="Starting mode [cli | gui | node | test]", metavar="MODE", default="cli")
(options, args) = parser.parse_args()

def node():
    DHNode()

def cli():
    DHNode(_mode=options.mode)

def gui():
    DHNode(_mode=options.mode)
    
def test():
    from tests import fulltest
    fulltest()

router = {
    "node": node,
    "cli": cli,
    "test": test,
    "gui": gui
}

if options.mode in router:
    router[options.mode]()
else:
    print("Use -h for help")
