
#include "TestCase.hpp"

// Create the test case
TEST::Case test("TestCase Tests");


int main(int argc, char ** argv)
{
    //  Check comparison stuff for float
    //
    {
        float lhs = 0.001;
        float rhs = 0.0001;

        bool result = test.float_eq(lhs, rhs, 0.1);

        if(!result)
        {
            test.fail("float_eq() failure : 0");
        }

        result = test.float_eq(lhs, rhs, 0.000001);

        if(result)
        {
            test.fail("float_eq() failure : 1");
        }
    }

    //  Check comparison stuff for double
    //
    {
        double lhs = 0.001;
        double rhs = 0.0001;

        bool result = test.double_eq(lhs, rhs, 0.1);

        if(!result)
        {
            test.fail("double_eq() failure : 0");
        }

        result = test.double_eq(lhs, rhs, 0.000001);

        if(result)
        {
            test.fail("double_eq() failure : 1");
        }
    }

    // If these fail the compilation will fail
    test.check_true_msg (true,  "check_true_msg(true)");
    test.check_false_msg(false, "check_false_msg(false)");
    test.check_true(true);
    test.check_false(false);

    // Template things
    test.check_equal(10, 10);
    test.check_equal("f", "f");
    test.check_equal(false, false);
    test.check_equal_msg(10,  10,  "check_equal_msg<int,  int> (22,10)");
    test.check_equal_msg("f", "f", "check_equal_msg<char, char>(\"d\",\"f\")");
    test.check_equal_msg(true, true,  "check_equal_msg<bool,  bool> (true,true)");
    test.check_not_equal(22, 10);
    test.check_not_equal("d", "f");
    test.check_not_equal(true, false);
    test.check_not_equal_msg(22,  10,  "check_equal_msg<int,  int> (22,10)");
    test.check_not_equal_msg("d", "f", "check_equal_msg<char, char>(\"d\",\"f\")");
    test.check_not_equal_msg(true,  false,  "check_equal_msg<bool,  bool> (true, false)");

    test.info("Tester code tested..");
    test.pass();
}