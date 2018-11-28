
import connector

def test_get(con):
    print("Testing all valid get")

    con.get({})

    con.get({
        "type": int
    })

    con.get({
        "type": tuple
    })

    con.get({
        "key": "Some_Key"
    })

    con.get({
        "type": int,
        "where": lambda x: True if x > 20 else False
    })

def test_getli(con):
    print("Testing all valid getli")

    con.getli({
        "key": "my_list",
        "index": 4
    })

def test_getdv(con):
    print("Testing all valid getdv")

    con.getdv({
        "key": "my_dict",
        "value": "value_in_my_dict"
    })

def test_set(con):
    print("Testing all valid set")

    con.set({
        "key": "New_List",
        "value": [1,2,3,4]
    })

    con.set({
        "key": "New_Dict",
        "value": {
            "red": [255,0,0],
            "green": [0,255,0],
            "blue": [0,0,255]
        }
    })

    con.set({
        "value": 42,
        "where": lambda x: isinstance(x, int)
    })

def test_setli(con):
    print("Testing all valid setli")

    con.setli({
        "key": "my_list",
        "value": "Inserted thing",
        "index": 30,
        "overflow": "append"
    })

    con.setli({
        "key": "my_other_list",
        "value": "Inserted thing",
        "index": 30,
        "overflow": "expand"
    })

    con.setli({
        "key": "another_list",
        "value": "Inserted thing",
        "index": 4
    })

def test_setdv(con):
    print("Testing all valid setdv")

    con.setdv({
        "key": "my_dict",
        "value": "something_in_my_dict"
    })


def test_invalid_get(con):
    print("Testing invalid get")
    con.get([])

    con.get({
        "type": "WRONG"
    })

    con.get({
        "key": int
    })

    def subvert(x):
        from time import sleep
        sleep(x)

    con.get({
        "type": int,
        "where": subvert
    })
    
def test_invalid_getli(con):
    print("Testing invalid getli")
    con.getli({
        "key": "my_list"
    })

def test_invalid_getdv(con):
    print("Testing invalid getdv")
    con.getdv({
        "key": "my_list"
    })

def test_invalid_set(con):
    print("Testing invalid set")
    con.set({
        "key": "func"
    })

def test_invalid_setli(con):
    print("Testing invalid setli")
    con.setli({
        "key": "my_list",
        "value": "Inserted thing",
        "index": -3,
        "overflow": "append"
    })

def test_invalid_setdv(con):
    print("Testing invalid setdv")

    con.setdv({
        "key": "my_dict",
        "value": "something_in_my_dict",
        "overflow": "No overflow allowed"
    })


con = connector.munconnect("127.0.0.1", 9001, "test", "a_basic_password", 'test_db')

con.connect()

test_get(con)
test_getli(con)
test_getdv(con)
test_set(con)
test_setli(con)
test_setdv(con)
test_invalid_get(con)
test_invalid_getli(con)
test_invalid_getdv(con)
test_invalid_set(con)
test_invalid_setli(con)
test_invalid_setdv(con)

con.disconnect()