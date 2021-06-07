

set(TEST_INCLUDES
    ${CMAKE_SOURCE_DIR}/tests
)

# Test the TestCase test helper before its relied on by other tests
add_subdirectory(testcase_tests)