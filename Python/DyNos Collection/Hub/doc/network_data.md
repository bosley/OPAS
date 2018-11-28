
# Network packet data

***All data is serialized when on network lines***

## Network packet (to server)
{
    "to": "server-mac.local/routine/slot",
    "from": "node-mac.local/routine/slot",
    "publicKey": "node_public_key",
    "signature": <signed "from" data>
    "route" <network-router map val>
    "data": <request_dict>
}
## Network packet (from server)
{
    "from": "server-mac.local/routine/slot",
    "publicKey": "server_public_key",
    "signature": <signed "from" data>
    "data" : <response_dict>
}



# Registration request_dict

{
    "caller": "mac-address",
    "slots": [
        {
            "name": "slot-name",
            "arguments": [
                (arg_name, str),
                (arg0_name, bool),
                (arg1_name, list)
            ],
            "returns": int
        },
        {
            "name": "slot0-name",
            "arguments": [
                (arg_name, str),
                (arg0_name, bool),
                (arg1_name, list)
            ],
            "returns": int
        }
    ],
    "routines": [
        {
            "name": "routine-name",
            "returns": int
        },
        {
            "name": "routine0-name",
            "returns": bool
        }
    ]
}

# Update to registration

Only performs update of the signature of the calling node matches the stored public key

