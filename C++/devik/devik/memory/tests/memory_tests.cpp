//
// Created by Josh Bosley on 6/3/21.
//
#include <variant>
#include <vector>

#include "memory/Memory.hpp"
#include "objects/Types.hpp"
#include "objects/Integer.hpp"
#include "objects/Double.hpp"
#include "objects/String.hpp"
#include "objects/Result.hpp"

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetectorNewMacros.h"

namespace
{
    // Test Case for the memory object
    struct TCMemoryObject
    {
        std::string var_name;
        VM::ObjectType type;
        std::variant<long, double, std::string> data;
        std::shared_ptr<VM::Object> obj;
    };

    //  Test objects to put in and pull out of the memory device
    //
    std::vector<TCMemoryObject> TCMO = {
            TCMemoryObject {
                "my_dbl",
                    VM::ObjectType::DOUBLE,
                    3.14,
                     std::make_shared<VM::Double>( VM::Double(3.14) )
                     },
            TCMemoryObject {
                "my_int",
                    VM::ObjectType::INTEGER,
                    (long)42,
                     std::make_shared<VM::Integer>( VM::Integer(42) )
                     },
            TCMemoryObject {
                "my_str",
                    VM::ObjectType::STRING,
                    "This is a string!",
                     std::make_shared<VM::String>( VM::String("This is a string!") )
                     },
            TCMemoryObject {
                "my_int1",
                    VM::ObjectType::INTEGER,
                    (long)9382838,
                     std::make_shared<VM::Integer>( VM::Integer(9382838) )
                     },
            TCMemoryObject {
                "my_dbl1",
                    VM::ObjectType::DOUBLE,
                    4234234.234232,
                     std::make_shared<VM::Double>( VM::Double(4234234.234232) )
                     },
            TCMemoryObject {
                "my_str1",
                    VM::ObjectType::STRING,
                    "This is another string for testing",
                     std::make_shared<VM::String>( VM::String("This is another string for testing") )
                     }
    };
}

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST_GROUP(MemoryTest)
{
    void setup(){}
    void teardown(){}
};

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST(MemoryTest, GetFromEmptyContext)
{
    // Create the object
    VM::Memory memory;

    // Remove the only context that it has
    memory.pop_context();

    // Attempt to get the item
    auto result = memory.get("some_var");

    CHECK_FALSE_TEXT(result->is_okay(), "Expected result.is_okay() to be false, but got true");
};

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST(MemoryTest, SimpleGetVariable)
{
    for(int step = 0 ; step < 100; step++)
    {
        // Create the object
        VM::Memory memory;

        // Set the variable to an integer with the _value of "step"
        memory.set("var", std::make_shared<VM::Integer>( VM::Integer(step) ));

        // Get the var
        auto result = memory.get("var");

        // Make sure its okay
        CHECK_TRUE_TEXT(result->is_okay(), "Expected result.is_okay() to be true, but got false");

        // Retrieve the inner _value
        auto inner = result->get_inner();

        // Lock it and get a shared
        if( auto sp = inner.lock() )
        {
            // Ensure we got back an integer
            CHECK_TRUE_TEXT( (sp->get_type() == VM::ObjectType::INTEGER), "Expected item to be integer, but it was not");

            // Don't do this when actually using this _type. Use the visitor
            VM::Integer* i = (VM::Integer*)sp.get();

            // Ensure that the _value of the integer is what we expect
            CHECK_EQUAL_TEXT((long)step, i->get_value(), "Value for retrieved variable did not match expected");
        }
        else
        {
            FAIL("Failed to obtain lock on weak pointer from result _type");
        }
    }
};

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST(MemoryTest, AllGetVariable)
{
    // For each test case
    for(auto & tc : TCMO )
    {
        // Create the object
        VM::Memory memory;

        // Set the variable to the test case object
        memory.set(tc.var_name, tc.obj);

        // Get the var
        auto result = memory.get(tc.var_name);

        // Make sure its okay
        CHECK_TRUE_TEXT(result->is_okay(), "Expected result.is_okay() to be true, but got false");

        // Retrieve the inner _value
        auto inner = result->get_inner();

        // Lock it and get a shared
        if( auto sp = inner.lock() )
        {
            // Ensure we got back the same _type that we put in
            CHECK_TRUE_TEXT( (sp->get_type() == tc.type), "Retrieved incorrect item _type");

            // Switch on the _type so we can cast it correctly ... kind of .. really a visitor should be
            // used but for the sake of the tests we are casting directly
            switch(tc.type)
            {
                case VM::ObjectType::INTEGER:
                {
                    CHECK_EQUAL_TEXT(
                            std::get<long>(tc.data),
                            ((VM::Integer*)sp.get())->get_value(),
                            "Value for retrieved variable did not match expected _type <int>");
                    break;
                }
                case VM::ObjectType::DOUBLE:
                {
                    CHECK_EQUAL_TEXT(
                            std::get<double>(tc.data),
                            ((VM::Double*)sp.get())->get_value(),
                            "Value for retrieved variable did not match expected _type <double>");
                    break;
                }
                case VM::ObjectType::STRING:
                {
                    CHECK_EQUAL_TEXT(
                            std::get<std::string>(tc.data),
                            ((VM::String*)sp.get())->get_value(),
                            "Value for retrieved variable did not match expected _type <string>");
                    break;
                }
                default:
                {
                    FAIL("Default case hit when matching test case types - Test needs to be updated")
                }
            }
        }
        else
        {
            FAIL("Failed to obtain lock on weak pointer from result _type");
        }
    }
};

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST(MemoryTest, ShadowVariables)
{
    VM::Memory memory;

    auto var_one = std::make_shared<VM::Double>( VM::Double(3.14) );
    auto var_two = std::make_shared<VM::Double>( VM::Double(983.22) );

    memory.set("dup", var_one);

    // Add a new context
    memory.new_context();

    // Shadow the variable
    memory.set("dup", var_two);

    auto result = memory.get("dup");

    CHECK_TRUE_TEXT(result->is_okay(), "Expected result.is_okay() to be true, but got false");

    auto inner = result->get_inner();

    if( auto sp = inner.lock() )
    {
        CHECK_TRUE_TEXT( (sp->get_type() == VM::ObjectType::DOUBLE), "Expected item to be double, but it was not");

        VM::Double* i = (VM::Double*)sp.get();

        // Make sure that we got the correct _value
        CHECK_EQUAL_TEXT(983.22, i->get_value(), "Value for retrieved variable did not match expected");
    }
    else
    {
        FAIL("Failed to obtain lock on weak pointer from result _type");
    }
};

// -------------------------------------------------------------------
//
// -------------------------------------------------------------------

TEST(MemoryTest, PopContextWithShadow)
{
    VM::Memory memory;

    auto var_one = std::make_shared<VM::Double>( VM::Double(3.14) );
    auto var_two = std::make_shared<VM::Double>( VM::Double(983.22) );

    memory.set("dup", var_one);

    // Add a new context
    memory.new_context();

    // Shadow the variable
    memory.set("dup", var_two);

    // Remove the newest context so we should get the first variable we set
    memory.pop_context();

    auto result = memory.get("dup");

    CHECK_TRUE_TEXT(result->is_okay(), "Expected result.is_okay() to be true, but got false");

    auto inner = result->get_inner();

    if( auto sp = inner.lock() )
    {
        CHECK_TRUE_TEXT( (sp->get_type() == VM::ObjectType::DOUBLE), "Expected item to be double, but it was not");

        VM::Double* i = (VM::Double*)sp.get();

        // Make sure that we got the correct _value
        CHECK_EQUAL_TEXT(3.14, i->get_value(), "Value for retrieved variable did not match expected");
    }
    else
    {
        FAIL("Failed to obtain lock on weak pointer from result _type");
    }
};