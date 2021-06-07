################################################
#   UtilTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslCoreUtilTests
        UTILMemBank.cpp
        UTILTimerTest.cpp
        UTILTimedCaller.cpp
        main.cpp
)
target_link_libraries(bslCoreUtilTests ${LIBRARY_NAME} ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET bslCoreUtilTests COMMAND ./bslCoreUtilTests POST_BUILD)