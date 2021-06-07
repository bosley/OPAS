cmake_minimum_required(VERSION 3.8)

project(BslCoreExampleUsage)

find_package(BslCore REQUIRED)
include_directories(${BSLCORE_INCLUDE_DIRS})

# build type
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
)

# add binary
add_executable(BslCoreExampleUsage main.cpp)

# add library
target_link_libraries(BslCoreExampleUsage ${BSLCORE_LIBRARIES})
