# Documentation

***Storage***

    User and server configuration storage
    
        /etc/mundis-server/
            config.db   ->  Server configuration
            users.db    ->  User configuration
            schemas.db  ->  List of user-defined databases

    User-defined and accessed databases 

        /var/lib/mundis-server/

    Logs
        /var/log/mundis-server.log


***Files Structure***

### mundis.py
    Entry point for launching the mundis server
    Setup for installation will run on first execution, but
    the user can re-configure the server by running --setup
        Available Commands:

            --version           => See version information
            --setup             => Perform setup
            --lsconfig          => Show setup information
            --uninstall         => Uninstall the server
            --cli               => Local user/database configuration
            --core
                start           => Start the daemon
                stop            => Stop the daemon
                restart         => Restart the daemon


### connector.py
    User-facing query class enables the usage of Mundis in python3 projects

### explorer.py
    Human interaction with a remote Mundis DB

### mundiscli.py
    Configuration of local server users and databases

### demiurge.py
    Daemonizer that manages the --core [start|stop|reset] functionality

### mundiscore.py
    Daemonized server functionality that controls the XMLRPC server. Logs
    to /var/log/mundis-server.log

### mundisserve.py
    XMLRPC Server

### mundisuser.py
    Handles user management and authentication

### mundisquery.py
    Validates user created queries

### mundissession
    User session handler

### mundidbobj.py
    Database wrapper for handling user queries on existing datbases

### mundiscrypto
    Simple AES en/decryptor and token generator

### /tests/*
    Testing files used for validating database actions