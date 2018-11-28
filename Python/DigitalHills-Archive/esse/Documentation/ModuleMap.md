# Description of modules:

## chain
    block.py
        Object structure of a block
    blockchain.py
        Blockchain logic
    mempool.py
        Shared queue for storing pending TX
    transaction.py
        Object structure of a transaction

## client
    client.py
        A command line client for use in the Esse network

    clientInterface.py
        Methods used for client development. Used in client.py, the use-case for
        this is in the development of wallets and gui clients for Esse 

## dhash
    methods.py
        Hashing and mining methods

## display
    color.py
        Terminal colors! 

## dpeer
    nexus.py
        Primary controller for handling peer connections 

    known_nodes.json
        List of previously active peers (Upated by nexus)

## node
    DHNode.py
        Thread manager and instance owner of blockchain and mempool. HCI logic exists here as well

## settings
    settings.py
        Holds block chain, and node settings Mostly meant for the local instance

    target.py
        Logic for handling difficulty adjustment and block rewards

## tcplib
    server.py
        Multithreaded socket server, uses nexus as the handling class for requests under the nexus method 'handle'

    transmitter.py
        Enables the connecting to other peers via the outGoingQuery method, exported to be accessed as 'query'

    handler.py
        Unused example of how a handler class should be setup 

## tests
    tests.py
        Used to test logic of different aspects of the blockchain

## entry.py
    The entry file to the whole program.
    Use -h for startup options!

    