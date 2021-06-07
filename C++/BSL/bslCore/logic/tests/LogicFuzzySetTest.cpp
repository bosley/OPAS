#include <iostream>

#include "LogicFuzzySet.hpp"

#include "CppUTest/TestHarness.h"

#define TOLERANCE       0.001
#define BOOL_TEST_SIZE  6
#define DBLE_TEST_SIZE  12

namespace {

    struct TestCaseBool {
        std::string name;
        bool        value;
    };

    struct TestCaseDouble {
        std::string name;
        double      value;
    };

    TestCaseBool tcBools[BOOL_TEST_SIZE] = { 
        {"bool1", true}, {"bool2", false}, {"bool3", true}, {"bool4", true}, {"bool5", false}, {"bool6", true}
     };

    TestCaseDouble tcDoubles [DBLE_TEST_SIZE] = {
        {"double1", 0.33}, {"double2", 0.5}, {"double3", 1.0}, {"double4",  0.21}, {"double5",  0.99}, {"double6",  0.19},
        {"double7", 0.33}, {"double8", 0.5}, {"double9", 1.0}, {"double10", 0.21}, {"double11", 0.99}, {"double12", 0.19},
    };
}

TEST_GROUP(FuzzySetTest)
{
};

TEST(FuzzySetTest, fsTest)
{
    LOGIC::FuzzySet firstSet;

    CHECK_EQUAL_TEXT(true, firstSet.isEmpty(), "Empty set indicates values present");
    CHECK_EQUAL_TEXT(0,    firstSet.size(),    "Set size not what was expected");

    for(auto tc : tcBools) {

        firstSet.insert(tc.name, tc.value);
    }

    CHECK_EQUAL_TEXT(false,          firstSet.isEmpty(), "Populated set indicates empty");
    CHECK_EQUAL_TEXT(BOOL_TEST_SIZE, firstSet.size(),    "Set size not what was expected");


    LOGIC::FuzzySet secondSet;

    CHECK_EQUAL_TEXT(true, secondSet.isEmpty(), "Empty set indicates values present");
    CHECK_EQUAL_TEXT(0,    secondSet.size(),    "Set size not what was expected");

    for(auto tc : tcDoubles) {

        secondSet.insert(tc.name, tc.value);
    }

    CHECK_EQUAL_TEXT(false,          secondSet.isEmpty(), "Populated set indicates empty");
    CHECK_EQUAL_TEXT(DBLE_TEST_SIZE, secondSet.size(),    "Set size not what was expected");

    // These will be impossible to test due to the undeterministic nature of the bools
    LOGIC::FuzzySet andset = firstSet && secondSet;
    LOGIC::FuzzySet orset  = firstSet || secondSet;
    LOGIC::FuzzySet notSet = !firstSet;

    for(auto tc : tcBools) {
        
        CHECK_EQUAL_TEXT(!firstSet[tc.name].get(), notSet[tc.name].get(), "Deterministic boolean value incorrect");
    }

    firstSet.clear();
    secondSet.clear();

    CHECK_EQUAL_TEXT(true, firstSet.isEmpty(), "Empty set indicates values present");
    CHECK_EQUAL_TEXT(true, secondSet.isEmpty(), "Empty set indicates values present");

    CHECK_EQUAL_TEXT(0, firstSet.size(),  "Set size not what was expected");
    CHECK_EQUAL_TEXT(0, secondSet.size(), "Set size not what was expected");
}