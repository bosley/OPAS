# Transmitter

Right now the transmitter basically just assumes that it didn't get a denied request from a server and goes on and sends the data. This needs to change. In the transmitter source, when receiving confirmation from the server it needs to checl if it received a 'CTCPLIB_STD_REPLY_CONFIRM' before moving on.

# Server

Oh boy. 

The server, while working as expected, needs to ensure some safety for the classes that register as receivers. Right now at the end of the connection thread the thing basically just jams the data into receiver without any concern for the receiver. Perhaps a middle structure could be placed in that the receiver can poll at its leisure. Idk. This needs to be looked at though.

In-addition to the jam, some refactoring of the server would do it good. I hate try-catch statements and right now the server looks a little muddled ensuring that the transmitter is actually sending interger values when it should be. Ironing this out with some check methods might be nice. Not really as urgent as the aforementioned jam though.

# Tests

Editing the chunk size in Common.h has been nice for testing out the chunking and sending of data from transmitter to server, and that is all well and good but some real tests should be put into place. I'm thinking something that looks at the CTCPLIB_STD_CHUNK_SIZE and laughs in the face of it and its god. *ahem* by that, of course, I mean a stress test. 

# CMake!

Maybe / probably / unfortunatly should be done prior to the tests. Because, lets be honest, CPPUTests being built and executed during the build is really the best way to go and ensure that the software stays not broken 

# More C-Like C++

Right now C and C++ are being made to jive together. You'd think that'd be okay, but its not. I'd like to move the C chunks from the sources into a C area specifically for them to be called on by the C++ bits. There is also a potential mangling issue right now. ***it works fine on my machine*** but that doesn't matter. It won't work everywhere so it is trash. 

# Debugging and whatnot

Oh my god I am NOT okay with the debug stuff that is stapled into these classes. Something much better needs to be put into place. I have a few ideas in mind, basically callbacks to the main, but I wanted this to be a 'SMALL' thing to just plop into projects.. but it might get bigger. 
