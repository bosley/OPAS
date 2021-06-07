#ifndef OBELUS_CONSTANTS
#define OBELUS_CONSTANTS

namespace OBELUS
{
    namespace CONSTANTS
    {
        //
        //  Network settings
        //
        static constexpr int   MAX_BUFFER_VALIDATION   = 10;        //! Buffer size for validation requests sent over the network
        static constexpr int   TCP_MAX_BUFFER_DATA     = 10000000;  //! Buffer size for data sent over the network (bytes) [ > 1]   

        //
        //  LUA
        //
        static constexpr int   MAX_LUA_WRITE_BURST      = 10;       //! Maximum number of messages to empty from outbound queue per tick
    }
}

#endif