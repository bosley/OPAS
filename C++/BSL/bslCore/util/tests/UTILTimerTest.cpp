
#include <chrono>
#include <iostream>
#include <thread>

#include "UtilTimer.hpp"

#include "CppUTest/TestHarness.h"

namespace
{
    double SECONDS[3] = {0.1, 0.5, 1.0};
}

TEST_GROUP(TimerTest)
{
};

TEST(TimerTest, sleepTest)
{
    for(int i = 0; i < 3; i++ ){

        UTIL::Timer timer(SECONDS[i]);

        CHECK_FALSE_TEXT(timer.isExpired(), "Timer expired before it should have");

        std::this_thread::sleep_for(std::chrono::duration<double>(SECONDS[i]));

        CHECK_TRUE_TEXT(timer.isExpired(), "Timer was not expired when it should have been");
    }
}

TEST(TimerTest, resetTest)
{
    for(int i = 0; i < 3; i++ ){

        UTIL::Timer timer(SECONDS[i]);

        CHECK_FALSE_TEXT(timer.isExpired(), "Timer expired before it should have");

        std::this_thread::sleep_for(std::chrono::duration<double>(SECONDS[i]));

        timer.reset();

        CHECK_FALSE_TEXT(timer.isExpired(), "Timer was expired when it should not have been");
    }
}

TEST(TimerTest, dumbTimer)
{
    for(int i = 0; i < 10; i++ ){
        UTIL::Timer timer;
        CHECK_TRUE_TEXT(timer.isExpired(), "Timer was not expired when it should have been");
    }
}