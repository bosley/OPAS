################################################
#   UtilTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslCoreMathTests
        MatrixTest.cpp
        main.cpp
)
target_link_libraries(bslCoreMathTests ${LIBRARY_NAME} ${CPPUTEST_LDFLAGS})


#####################
# Run tests
#####################
add_custom_command(TARGET bslCoreMathTests COMMAND ./bslCoreMathTests POST_BUILD)