
#include <chrono>
#include <iostream>
#include <thread>

#include "UtilTimer.hpp"
#include "UtilTimedCaller.hpp"

#include "CppUTest/TestHarness.h"

namespace
{
    class Callable : public UTIL::TimedCallableIf {
        public:
            Callable(double time, bool repeated) : called(false), timer(time), repeated(repeated), ignore(false){

            }

            void call() override {
                called = true;
            }

            bool        called;
            UTIL::Timer timer;
            bool        repeated;
            bool        ignore;
    };
    
    Callable testCases[3] { 
        Callable(0.1, true), Callable(0.2, false), Callable(0.3, true)
    };
}

TEST_GROUP(TimedCallerTest)
{
};

TEST(TimedCallerTest, checkCalls)
{
    UTIL::TimedCaller caller;

    for(auto &tc : testCases) {
        caller.registerCallable(tc.timer, tc, tc.repeated);
    }

    // This timer should be only slightly higher than the sum of the test cases
    UTIL::Timer testTimer(0.4);

    while(!testTimer.isExpired()) {

        caller.tick();
    }

    for(auto &tc : testCases) {

        CHECK_EQUAL_TEXT(true, tc.called, "A callable was not called when its timer was expired");
    }
}
