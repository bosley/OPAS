//
// Created by Josh Bosley on 6/3/21.
//

#include "CppUTest/CommandLineTestRunner.h"

int main(int ac, char** av)
{
    //MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return CommandLineTestRunner::RunAllTests(ac, av);
}