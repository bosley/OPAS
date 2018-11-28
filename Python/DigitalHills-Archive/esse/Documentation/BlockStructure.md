# Expected block structure 

|-Header----------------------|
|   Version                   |
|   Time Released             |
|   Time Created              |
|   Nonce                     |
|   Previous Hash             |
|   Merkel Root               |
|_____________________________|

|-Transactions-|
| 1            |
| .            |
| .            |
| N            |
|______________|


# Transactions 

## Types

----- Type ------------------ Code ---------
    One to many     |           0
    Many to one     |           1
    One to One      |           2
--------------------------------------------

Many -> Many will be invalid

## Options

overflow - True | False
    If one of the accounts in (many-to-one does not have enough to complete tx, overflow to pull from another account)
    Default state = False

## Structure

{
    "timestamp": "time",
    "identification": "some.unique.id",
    "type": (<type>, overflow=False),
    "from": [<from> .. <fromN>],
    "to": [<to> .. <toN>]
}