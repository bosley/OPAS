
#include <chrono>
#include <iostream>
#include <thread>

#include "UtilMemBank.hpp"

#include "CppUTest/TestHarness.h"


TEST_GROUP(MemBankTest)
{
};

TEST(MemBankTest, sleepTest)
{
    UTIL::MemBank<int> test(200);

    int* failure = test.getEntryIdx(44);

    if(failure != nullptr) {
        FAIL_TEST("An expected failure didn't fail, so the failure failed to fail");
    }

    for(int i = 0; i < 500; i++){
        if(!test.pushData(i)){
            FAIL_TEST("Failed to set data into bank");
        }
    }

    for(uint64_t i = 0; i < 100; i++){
        int* data = test.getEntryIdx(i);

        if(data == nullptr){
            FAIL_TEST("Failed to get _any_ data");
        } else {
            if(*data != static_cast<int>(i)+300) {
                CHECK_EQUAL_TEXT(i+300, *data, "Got incorrect data");
            }
        }
    }

    test.clear();

    for(int i = 0; i < 500; i++){
        if(!test.pushData(i)){
            FAIL_TEST("Failed to set data into second bank set");
        }
    }

    std::vector<int> frame = test.getFrame(0, 200);

    int val = 300;
    for(auto i = frame.begin(); i != frame.end(); ++i) {
        if(val++ != (*i)) {
            CHECK_EQUAL_TEXT(val, (*i), "Got incorrect data");
        }
    }
}
