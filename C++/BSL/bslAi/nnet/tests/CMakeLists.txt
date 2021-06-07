################################################
#   NnetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(bslAiNnetTests
        BuilderTest.cpp
        JsonTest.cpp
        main.cpp
)
target_link_libraries(bslAiNnetTests 
                      ${LIBRARY_NAME} 
                      ${CPPUTEST_LDFLAGS}
                      ${BSLCORE_LIBRARIES}
                      nlohmann_json::nlohmann_json)

add_custom_command(
        TARGET bslAiNnetTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/nnet/tests/testTrain.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTrain.json)

add_custom_command(TARGET bslAiNnetTests COMMAND ./bslAiNnetTests POST_BUILD)