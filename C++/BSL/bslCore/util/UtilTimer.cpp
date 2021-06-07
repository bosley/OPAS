#include "UtilTimer.hpp"

namespace UTIL
{
    // -------------------------------------------------
    // Timer
    // -------------------------------------------------
    Timer::Timer() : seconds(0) {
    }

    // -------------------------------------------------
    // Timer
    // -------------------------------------------------
    Timer::Timer(double seconds) : seconds(seconds),
                                   start(std::chrono::system_clock::now()) {
    }

    // -------------------------------------------------
    // ~Timer
    // -------------------------------------------------
    Timer::~Timer(){

    }

    // -------------------------------------------------
    // isExpired
    // -------------------------------------------------
    bool Timer::isExpired() {
        return ( (std::chrono::system_clock::now() - start) > seconds) ? true : false;
    }

    // -------------------------------------------------
    // reset
    // -------------------------------------------------
    void Timer::reset() {
        start = std::chrono::system_clock::now();
    }
}