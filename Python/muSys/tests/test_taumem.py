from musys import memsess
from musys import netmem

port = 9001
session_time = 10 # Seconds
udi = "node-test-"
auth = "Some_auth_token_x112358x256"

NAM = netmem(auth, 9001)
NAM.daemon = True
NAM.start()

my_session = memsess(auth, port, udi, 10)
my_session.beginSession()

my_session.put("label", "TESTDATA")
my_session.put("label0", "TESTDATA0")
my_session.put("label1", "TESTDATA1")

ret = my_session.fetch("label")
print(ret)

h = input("Input Block - Wait until server info pops\n")


print("This should give key not found.")
ret = my_session.fetch("label")
print(ret)

del my_session

new_session = memsess(auth, port, "NEWUDI", 3, reset_timeout=True)
new_session.beginSession()

new_session.put("label", "TESTDATA")
new_session.put("label0", "TESTDATA0")
new_session.put("label1", "TESTDATA1")

ret = new_session.fetch("label")
print(ret)

h = input("Input Block - Wait until server info pops\n")

print("This should give key not found.")
ret = new_session.fetch("label")
print(ret)

del new_session

print("Creating new session")

new_session = memsess(auth, port, "NEWUDI", 10)
new_session.beginSession()

new_session.put("label", "TESTDATA")
new_session.put("label0", "TESTDATA0")
new_session.put("label1", "TESTDATA1")

ret = new_session.fetch("label")
print(ret)

print("Manually closing session")
new_session.closeSession()

print("This should give None - Session not active")
ret = new_session.fetch("label")
print(ret)

print("Resarting session")
new_session.beginSession()

print("This should give key not found. - No previous session info should exist")
ret = new_session.fetch("label")
print(ret)

print("Adding new data")
new_session.put("label", "TESTDATA")
new_session.put("label0", "TESTDATA0")
new_session.put("label1", "TESTDATA1")

ret = new_session.fetch("label1")
print(ret)

print("Manually closing session again")
new_session.closeSession()

del new_session