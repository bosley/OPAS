#ifndef TYPES_H
#define TYPES_H

#include <chrono>

namespace MOD { namespace TYPES {

typedef std::chrono::system_clock sysclock;
typedef std::chrono::time_point<std::chrono::system_clock> ctpoint;

} }

#endif // TYPES_H
