################################################
#   NetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(NetTests
        AllNetTests.cpp
        main.cpp
)

target_link_libraries(NetTests
    PUBLIC 
        butane
        ${CPPUTEST_LDFLAGS}
)

#####################
# Run tests
#####################
add_custom_command(TARGET NetTests COMMAND ./NetTests POST_BUILD)