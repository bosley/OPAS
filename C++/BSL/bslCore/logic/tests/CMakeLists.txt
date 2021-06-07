################################################
#   UtilTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslCoreLogicTests
        LogicFuzzyBooleanTest.cpp
        LogicFuzzySetTest.cpp
        main.cpp
)
target_link_libraries(bslCoreLogicTests ${LIBRARY_NAME} ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET bslCoreLogicTests COMMAND ./bslCoreLogicTests POST_BUILD)