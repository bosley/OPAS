

Each message chunk will use \n as delim
The first part of every message will be used to determine
what the specific request type is, with the following line(s) 
as data for the request.

## Information Pushes (IP)
> For use when local node needs to broadcast infromation to 
> all peers

chunk 0:
        "broadcast"
chunk 1: 
        "type of broadcast"
chunk 2:
        "JSON data"

Accepted broadcasts: 
    - All chunk 0's should be "broadcast"

    IP.0 
    "new_client"\n
    "{
        "address": "<address>",
        "port": <port>,
        "lastConnect": "<UTC LAST CONNECT>",
        "failedPings": <#>,
        "online": <bool>,
        "epochTime": <UTC>,
        "personalTime": <UTC>
    }"

>   Inform the network that the current node has joined the network

    IP.1
    "new_transaction"\n
    "<Stringed JSON of TX"\n

>   Inform the network that the current node has a new transaction

    IP.2
    "block_submitted"\n
    "<Stringed JSON of newest BLOCK>"\n

>   Inform the network that someone submitted a block

## Syncronization Requests (SR)
> Send / Receive a request to synchronize some aspect of the 
> node to / from a peer

chunk 0:
        "synchronize"
chunk 1:
        "type of synchronization"
chunk 2:
        "JSON data"

    SR.0
    "blockchain"\n
    "{
        "head": "Current Head",
        "height": 0
    }"\n

>   A request to sync blockchain, given the current head. If the head listed
>   in the request is the same as the local head, no sync is required

    SR.1
    "mempool"\n
    "{
        "memhash": "A hash of the mem pool",
        "mempool": [List, of, all tx]
    }"\n

>   Sync the memory pool. Ensure that both pools have a single copy of all transactions
>   Once a memory pool is synced

    SR.2
    "peers"\n
    "JSON of peer list"\n

>   Sync the list of peers between two nodes

## Information Request (IR)
> Send / Receive a request about something simple that may be used in, but
> is not exclusive to a SR. No description given, as the returned result should
> be apparent

chunk 0:
        "information"
chunk 1:
        "specific member"

    IR.0
    "timestamp"\n

    IR.1
    "numBlocks"\n

    IR.2
    "numPeers"\n

    IR.3
    "numPool"\n

    IR.4
    "uptime"\n

## Mining Interactions
> Request for block, and submissions
> Responses from network denoted by '-R'

chunk 0:
        "miner"
chunk 1:
        "<Miner Address>"\n

    MI.0
    "{
        "type": "req_block",
        "txCount": <Max number of transactions for block>,
    }"

        MI.0-R
        "{
            "network": <version>,
            "head": <current head>,
            "released": <time-released>,
            "transactions": <JSON of TX>,
        }"
    
    MI.1
    "{
        "type": "req_block_submission",
        "block": <JSON Block data>
    }"