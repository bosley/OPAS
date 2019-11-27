
# Gateway

Exist on a cluster and are also public. Gateways sync their known local nodes and provide a means to transport requests to/from nodes on other clusters.

# Node

Existing in clusters (lose definition, just means they have a means to connect to each other), and able to contact remote clusters by-way of their local gateway, or directly to nodes on the same network.

### Example Network

```
Node_0 ---- Node_1 ---- Node_2
   |          |          |
    \_________|_________/        
              |                               _____ Node_8
          Gateway_0 ----\                    /        |
              |          \____ Gateway_2 ___/______ Node_7
              |          /                  \         |
          Gateway_1 ----/                    \_____ Node_6
              |
     _________|_________
    /         |         \
Node_3 ---- Node_4 ---- Node_5   
```

- Gateway 0, 1, 2 Synchronize their known nodes into a network map
- Node_0 can access Node_2 root via     : /local/Node_2_uuid/
- Node_0 can access Node_2 method via   : /local/Node_2_uuid/{method}
- Node_0 can access Node_7 root via     : /remote/Node_8
- Node_0 can access Node_7 method via   : /remote/Node_8/{method}

When _Node_0_ attempts to access _/remote/Node_8/_ the local gateway _Gateway_0_ will confirm that a route exists to Node_8 and establish direct communication to Node_8 via Gateway_2. The request to Node_8 will ba handed the reverse path to reach Node_0 : _/Gateway_2/Gateway_0/response/response-id. Node_8 when responding will write to this path and the response will be fullfilled for Node_0.
