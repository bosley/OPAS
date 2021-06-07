#include "bslcore/UtilTimer.hpp"

#include <iostream>

int main() {

    std::cout << "Example usage - A timer" << std::endl;

    UTIL::Timer timer(10);

    std::cout << "Waiting for 10 second timer..." << std::flush;

    while(!timer.isExpired()) {}

    std::cout << "Timer expired" << std::endl;;

    return 0;
}