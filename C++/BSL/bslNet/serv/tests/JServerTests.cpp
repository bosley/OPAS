//
// Created by bosley on 3/23/20.
//

#include "ServObjTransport.hpp"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ObjTransportTest)
{
};

TEST(ObjTransportTest, basic)
{
    SERV::ObjTransport serv("test server", "127.0.0.1", 4095);

}