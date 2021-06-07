#include "UtilTimedCaller.hpp"

namespace UTIL
{
    // -------------------------------------------------
    // TimedCaller
    // -------------------------------------------------
    TimedCaller::TimedCaller() {

    }

    // -------------------------------------------------
    // TimedCaller
    // -------------------------------------------------
    TimedCaller::TimedCaller(uint64_t reserve) {

        callList.reserve(reserve);
    }

    // -------------------------------------------------
    // ~TimedCaller
    // -------------------------------------------------
    TimedCaller::~TimedCaller() {
        
        callList.clear();
    }

    // -------------------------------------------------
    // registerCallable
    // -------------------------------------------------
    void TimedCaller::registerCallable(UTIL::Timer timer, TimedCallableIf& callable, bool repeat) {

        callList.push_back({ timer, &callable, repeat});
    }

    // -------------------------------------------------
    // tick
    // -------------------------------------------------
    void TimedCaller::tick() {

        for(auto i = callList.begin(); i != callList.end(); ++i) {

            if ((*i).callTimer.isExpired()) {

                (*i).callable->call();

                if(!(*i).repeat) {

                    callList.erase(i--);
                } else {

                    (*i).callTimer.reset();
                }
            }
        }
    }
}