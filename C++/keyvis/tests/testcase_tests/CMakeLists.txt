
add_executable(headtest
        ${KEYVIS_SOURCES}
        main.cpp
)
target_include_directories(headtest
    PRIVATE
        ${TEST_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
)

add_custom_command(TARGET headtest COMMAND ./headtest POST_BUILD)