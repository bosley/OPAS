add_executable(AnnBasicExample
        example.cpp
)
target_link_libraries(AnnBasicExample
    ${LIBRARY_NAME}
    ${CPPUTEST_LDFLAGS}
    ${BSLCORE_LIBRARIES}
    nlohmann_json::nlohmann_json
)

add_custom_command(
        TARGET AnnBasicExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/ann/tests/testTopology.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTopology.json)

add_custom_command(
        TARGET AnnBasicExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/ann/tests/testTrain.json
                ${CMAKE_CURRENT_BINARY_DIR}/testTrain.json)