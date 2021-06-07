cmake_minimum_required(VERSION 3.8)

project(Orbit)

find_package(BslNet REQUIRED)
include_directories(${BSNET_INCLUDE_DIRS})

##################################################
# Options
##################################################

option(COMPILE_TESTS    "Compile the tests"    ON)

##################################################
# Find CPPU test, and set project env
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(LIBS ${LIBS} ws2_32.lib)
else()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
endif(MSVC)

##################################################
# Modules
##################################################

set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Agent.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/HostPort.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Request.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RequestHandler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Response.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.hpp
        )

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Agent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/HostPort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Request.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Response.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

##################################################
# Configure Library
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)

##################################################
# Tests and Examples
##################################################

if(COMPILE_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

##################################################
# Configure Install
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)


