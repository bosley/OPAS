
# TODO


1. Define all RPC server functionality, and implement as it relates to blocker
    This will entail further construction and completion of the Blocker class
    - Reading / parsing the chain.blk file efficiently 

2. Create a Dynos-DNS system to handle the resolving of Dynos host names, 
   to their last-known IPv4 addresses.
   - This should be done in-tandem with seed handler, and network speed listings
     to find fastest possible nodes

3. Define all RPC client functionality. Query remote nodes, etc. 
   - Pulling in new blocks sent from external nodes
   - Light nodes only keep their own chain, full nodes will replicate
     everything for network propagation / archiving
     - Set limiter on chain length via configuration file for disk-constrained
       systems.

4. Pillar/Master node functionality for public-access nodes that will serve
    as the routers of information amongst the dynos network





