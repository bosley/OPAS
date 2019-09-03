#ifndef CTCPLIB_TYPES_H
#define CTCPLIB_TYPES_H

#include <string>
#include <vector>

namespace CTCP
{
    typedef std::vector<std::string> Chunks;

    struct MessageData
    {
        std::string ipFrom;
        int         portFrom;
        Chunks      chunkData;
    };
}

#endif
