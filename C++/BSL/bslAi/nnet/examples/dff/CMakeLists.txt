add_executable(DFFExample
        example.cpp
)
target_link_libraries(DFFExample
    ${LIBRARY_NAME}
    ${CPPUTEST_LDFLAGS}
    ${BSLCORE_LIBRARIES}
    nlohmann_json::nlohmann_json
)

add_custom_command(
        TARGET DFFExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/nnet/tests/testTrain.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTrain.json)