# Networks

Each network consists of two chunks. The name of the network, and the specific
network identifier.

>    <network>.<identifier>

The network tells the node to operate under specific assumptions, while the id
will indicate which nodes it will talk to, and which nodes to ignore.

The identifier is arbitrary, with the exception of the global network:

>   global.a001

The global network identifier must be a001 to identify that the node wants to interact
with the public global network.

## Dev
>    Strictly meant for local development.
>    Target will not adjust on this network, and the node will be operating
>    under the assumption that the network is ephimeral.
>
>    Example:       dev.a001

## Test
>    Live, ephimeral network. No initial synchronization with peers.
>    All TX lost when local network node goes offline. 
>    Basically the dev network, but includes target adjustments.
>
>    Example:        test.a001

## Global
>    Live, permanent network. 
>    All functionality will be in-place, all TX are final
>
>    Example:        global.a001