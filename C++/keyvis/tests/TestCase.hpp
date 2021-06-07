/*
    Josh Bosley

    This header file is a simple means to allow test cases to be written for unit testing. 
*/

#ifndef TEST_CASE_HPP
#define TEST_CASE_HPP

#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>

namespace TEST
{

//  A test object that allows named tests to pass, fail, and give info messages
//  - When the object is destructed it indicates that the test is "completed" 
//  - If "fail" is called the program gives any given message then exits with EXIT_FAILURE
//  - If "pass" is called the program gives any given message then exits with EXIT_SUCCESS
class Case
{
public: 

    //  Create the test object - The name will be used for output information
    //
    Case(std::string test_name) : name(test_name) 
    {
        std::cout << std::endl << "Test < " << name << " > " << std::endl;
        this->sep();
    }

    //  Destruct the test object and indicate complete
    //
    ~Case()
    {
        this->complete();
    }

    //  Cause the test to fail with a given message
    //
    void fail(std::string message)
    {
        std::cerr << "FAILURE : " << message << std::endl;
        ::exit(EXIT_FAILURE);
    }

    //  Cause the test to pass with a given message
    //
    void pass()
    {
        std::cout << "PASS" << std::endl;
        ::exit(EXIT_SUCCESS);
    }

    //  Display some information
    //
    void info(std::string message)
    {
        std::cout << ">> " << message << std::endl;
    }

    //  Check if 2 floats are approx. equal
    //
    bool float_eq(float lhs, float rhs, float epsilon)
    {
        return ::fabs(lhs - rhs) < epsilon;
    }

    //  Check if 2 doubles are approx. equal
    //
    bool double_eq(double lhs, double rhs, double epsilon)
    {
        return ::fabs(lhs - rhs) < epsilon;
    }

    //  Ensure something is true
    //
    void check_true_msg(bool value, std::string message)
    {
        if( !value )
        {
            this->fail(message);
        }
    }

    //  Ensure something is false
    //
    void check_false_msg(bool value, std::string message)
    {
        if( value )
        {
            this->fail(message);
        }
    }

    //  Ensure something is true
    //
    void check_true(bool value)
    {
        if( !value )
        {
            this->fail("Given item was 'false' when 'true' was expected");
        }
    }

    //  Ensure something is false
    //
    void check_false(bool value)
    {
        if( value )
        {
            this->fail("Given item was 'true' when 'false' was expected");
        }
    }

    //  Ensure some items of type 'T' are equal
    //
    template<class T>
    void check_equal_msg(T lhs, T rhs, std::string message)
    {
        if( lhs != rhs )
        {
            this->fail(message);
        }
    }

    //  Ensure some items of type 'T' are equal
    //
    template<class T>
    void check_equal(T lhs, T rhs)
    {
        if( lhs != rhs )
        {
            this->fail("Given items not equal when equality was expected");
        }
    }

    //  Ensure some items of type 'T' are not equal
    //
    template<class T>
    void check_not_equal_msg(T lhs, T rhs, std::string message)
    {
        if( lhs == rhs )
        {
            this->fail(message);
        }
    }

    //  Ensure some items of type 'T' are not equal
    //
    template<class T>
    void check_not_equal(T lhs, T rhs)
    {
        if( lhs == rhs )
        {
            this->fail("Given items equal when equality was not expected");
        }
    }


private:
    std::string name;

    // Make sure we keep the same width of a line everywhere
    void sep()
    {
        std::cerr << "----------------------------------------" << std::endl;
    }

    // Indicate complete 
    void complete()
    {
        this->sep();
        std::cout << ":: Complete < " << name << " > " << std::endl << std::endl;
    }
};

}


#endif