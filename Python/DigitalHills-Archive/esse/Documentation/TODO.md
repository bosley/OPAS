# Each section will be listed in-terms of the following depending on how specific the work to be done is:
>
>     module.file.method OR module.file OR module
>

0)
## client
        - Checkup on TX
        - Create Many-Many tx type (Front-end specification)

1) 
## Gen. TODO
    Create by hand (Come up with):
        Block releasment method - mempool
2)
## mempool.

    request_for_block
        Determine if block is ready given the yet-to-be-developed rules on block releasing

3) 
## Mining Platform

    Create a platform for mining (Pulling block from network, and mining)

Continuous)
## dpeer.nexus.handle
    Develop request protocol  - loosly (Develop as needed)

    Implement incomming request handling method that can 
    be expanded on as protocol developes

## dpeer.nexus.run && dpeer.nexus.__pushEventsToPeers
    Implement outgoing requests to peers. Develop handling and 
    outgoing requests in tandem to ensure each functions as intended
