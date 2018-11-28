
# Mundis Syntax
Mundis connections tie the session directly to a database, forcing the user to
one database interaction per-connection. 

Syntax examples assume that the user has defined the following:

```python
import connector
con = connector.munconnect(
    "address", 9001, 'server_encryption_token', 'user', 'password', 'database'
    )
con.connect()
```

Selection of data from a database

***Get***

Retrieve all items from database
    con.get({})

Retrieve all items mapped to a specific data type
Returns list of objects of defined type
    con.get({
        "type": int
    })

    con.get({
        "type": tuple
    })

Retrieve all keys mapped to a specific data type
Returns list of keys
    con.getKeys({
        "type": int
    })

Retrieve specific key(s)
    con.get({
        "key": "My_Key"
    })

    con.get({
        "key": "My_other_key"
    })

Retrieve ints
    con.get({
        "type": int
    })

Retrieve item's list index
    con.getli({
        "key": "my_list",
        "index": 4
    })

Retrieve dict item's value by key
    con.getdv({
        "key": "my_dict",
        "value": "something_in_my_dict"
    })

***Set***

All sets that occur on data not yet existing will be created

Set a value to a key
    con.set({
        "key": "Answer_To_Everything",
        "value": 42
    })

Set item's list index
    con.setli({
        "key": "my_list",
        "index": 4,
        "value": "Inserted thing"
    })

Set item's list index, with overflow as append
    - Set a list index, and if it's bigger than the list,
      it will append the list

    con.setli({
        "key": "my_list",
        "index": 4,
        "value": "Inserted thing",
        "overflow": "append"
    })

Set item's list index, with overflow as expand 
    - Will expand the item to overflow size to place item
      in requested index

    con.setli({
        "key": "my_list",
        "index": 30,
        "value": "Inserted thing",
        "overflow": "expand"
    })

Set dict item's value by key
    con.setdv({
        "key": "my_dict",
        "value": "something_in_my_dict"
    })
