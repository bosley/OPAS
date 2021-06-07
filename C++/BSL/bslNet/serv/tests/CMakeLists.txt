################################################
#   NetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslServTests
        JServerTests.cpp
        main.cpp
)
target_link_libraries(bslServTests
        ${LIBRARY_NAME}
        ${CPPUTEST_LDFLAGS}
        ${BSLCORE_LIBRARIES}
        nlohmann_json::nlohmann_json)


#####################
# Run tests
#####################
add_custom_command(TARGET bslServTests COMMAND ./bslServTests POST_BUILD)