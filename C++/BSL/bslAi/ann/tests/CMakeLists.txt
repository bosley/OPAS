################################################
#   AnnTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslAiTestsAnn
        TopologyTest.cpp
        NetworkTest.cpp
        JsonTest.cpp
        TrainingDataTest.cpp
        main.cpp
)
target_link_libraries(bslAiTestsAnn
                      ${LIBRARY_NAME} 
                      ${CPPUTEST_LDFLAGS}
                      ${BSLCORE_LIBRARIES}
                      nlohmann_json::nlohmann_json)


#####################
# Run tests
#####################
add_custom_command(
        TARGET bslAiTestsAnn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/ann/tests/testTopology.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTopology.json)

add_custom_command(
        TARGET bslAiTestsAnn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/ann/tests/testTrain.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTrain.json)

add_custom_command(TARGET bslAiTestsAnn COMMAND ./bslAiTestsAnn POST_BUILD)