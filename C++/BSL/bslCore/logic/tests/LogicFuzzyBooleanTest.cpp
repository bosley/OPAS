#include <iostream>

#include "LogicFuzzyBool.hpp"

#include "CppUTest/TestHarness.h"

#define TOLERANCE 0.001

TEST_GROUP(FuzzyBooleanTest)
{
};

TEST(FuzzyBooleanTest, fbTest)
{
    for(double i = 1.0; i < 10.0; i += 1.0) {
        double probability = (i/10.0);
        LOGIC::FuzzyBool fuzzyBool(probability);
        CHECK_EQUAL_TEXT(probability, fuzzyBool.get(), "FuzzyBool did not correctly set the probability");
    }

    LOGIC::FuzzyBool fbool(0.8);
    LOGIC::FuzzyBool nbool(0.2);

    DOUBLES_EQUAL(nbool.get(), (!fbool).get(),       TOLERANCE);
    DOUBLES_EQUAL(fbool.get(), (!nbool).get(),       TOLERANCE);
    DOUBLES_EQUAL(fbool.get(), (fbool||nbool).get(), TOLERANCE);
    DOUBLES_EQUAL(1.0,         (fbool||true).get(),  TOLERANCE);
    DOUBLES_EQUAL(fbool.get(), (fbool||false).get(), TOLERANCE);
    DOUBLES_EQUAL(nbool.get(), (fbool&&nbool).get(), TOLERANCE);
    DOUBLES_EQUAL(fbool.get(), (fbool&&true).get(),  TOLERANCE);
    DOUBLES_EQUAL(0.0,         (fbool&&false).get(), TOLERANCE);
}