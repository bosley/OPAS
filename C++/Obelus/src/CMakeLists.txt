cmake_minimum_required(VERSION 3.8)


include_directories(common/include)

set(COMMON_SOURCES
     ${PROJECT_SOURCE_DIR}/common/src/Formatter.cpp
     ${PROJECT_SOURCE_DIR}/common/src/Wrapper.cpp
     ${PROJECT_SOURCE_DIR}/common/src/Callbacks.cpp
)

add_subdirectory(cli)
add_subdirectory(node)