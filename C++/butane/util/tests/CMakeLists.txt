################################################
#   UtilTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(UtilTests
        FuzzyBooleanTest.cpp
        main.cpp
)

target_link_libraries(UtilTests
    PUBLIC 
        butane
        ${CPPUTEST_LDFLAGS}
)

#####################
# Run tests
#####################
add_custom_command(TARGET UtilTests COMMAND ./UtilTests POST_BUILD)