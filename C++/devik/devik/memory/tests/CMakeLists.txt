add_executable(memory_test
        ${SOURCES}
        memory_tests.cpp
        main.cpp
        )

include_directories(
        ${CPPUTEST_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(memory_test
        ${CPPUTEST_LIBRARIES}
        ${CPPUTEST_LDFLAGS}
        )

target_compile_options(memory_test PRIVATE -Werror)
target_compile_features(memory_test PRIVATE cxx_std_17)

add_custom_command(TARGET memory_test COMMAND ./memory_test POST_BUILD)