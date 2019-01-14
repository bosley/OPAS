# TODO

This is a loose list of TODOs because I don't use a free task planner or anything enough.

## Things I want to do
- Create a thing for TCP/UDP or whatever that can be dropped in as readers/writers to any implementation of the bus
- Figure out how to do a thing to communicate over shared memory without that boost nonsense that can be plopped into any implementation
- Take the memory thing and network thing listed above and redo it to make something similar to the connection ambiguous binding project I did in python that one time
- Shake-up TSGP implementation to make the whole getting messages from the bus more efficient

## Kind of important
- Create a an object and iface that can be used to encode/decode message data into a bytes/bits object for message data
- Add pub/priv key either to addresses or to an implementation that abstracts an address so we don't talk to strangers

## Not that important
- Make the cmakelists less ghetto
- Reformat the directories maybe to have tests and examples subdird into different implementation areas. 
- Add Doxygen stuff in the cmakelists so the awesome comments are actually used to create documents
