# HPIC - High priority inter-app communication 

This README describes how information is formatted for the HPIC server. This server is not able to be customized, 
and only useful for developing on the node further


Available commands : 

    getDesc 

    getVersion

    getLuaInfo

Formatting requests : 

```json

    {"hpic" : "getDesc"}

    {"hpic" : "getVersion"}

    {"hpic" : "getLuaInfo"}
```

Formatted responses within the data field of the response

```json

    {"desc" : "Server Description" }

    {"version" : "Server Version" }

    {"ip" :  "<ip address>" }
    {"port": "<port number>"}


```