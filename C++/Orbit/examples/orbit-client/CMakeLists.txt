cmake_minimum_required(VERSION 3.8)

find_package(Orbit REQUIRED)
include_directories(${ORBIT_INCLUDE_DIRS})

find_package(BslNet REQUIRED)
include_directories(${BSNET_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
        )

set(CMAKE_CXX_STANDARD 17)

add_executable(OrbitClient
        example.cpp
        )

target_link_libraries(OrbitClient
        PRIVATE
        ${ORBIT_LIBRARIES}
        ${BSLNET_LIBRARIES}
        )
