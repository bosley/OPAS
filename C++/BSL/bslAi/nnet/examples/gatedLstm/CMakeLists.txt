add_executable(GatedLSTMExample
        example.cpp
)
target_link_libraries(GatedLSTMExample
    ${LIBRARY_NAME}
    ${CPPUTEST_LDFLAGS}
    ${BSLCORE_LIBRARIES}
    nlohmann_json::nlohmann_json
)

add_custom_command(
        TARGET GatedLSTMExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/nnet/tests/testTrain.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTrain.json)