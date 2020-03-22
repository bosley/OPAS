cmake_minimum_required(VERSION 3.8)

project(butane VERSION 0.0.1 LANGUAGES CXX)

################################################
#     Project Options
################################################

if (MSVC)
  set(LIBS ${LIBS} ws2_32.lib)
else()
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
endif(MSVC)

option(COMPILE_EXAMPLES "Compile the examples" ON)
option(COMPILE_TESTS    "Compile the tests"    ON)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(">>> Building debug mode")
    add_compile_options( -D BUTANE_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(">>> Building release mode")
else()
    message(">>> Undefined build type")
endif()

#####################
# Find CPPUTEST
#####################
if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

################################################
#   NET
################################################

set(NET_INCLUDE
    ${CMAKE_CURRENT_LIST_DIR}/net/include/
)

set(NET_SOURCES
    net/src/NETSocket.cpp
    net/src/NETTcpServer.cpp
    net/src/NETUdpServer.cpp
    net/src/NETWriter.cpp
)

################################################
#   UTL
################################################

set(UTIL_INCLUDE
    ${CMAKE_CURRENT_LIST_DIR}/util/include/
)

set(UTIL_SOURCES
    util/src/UTILFuzzyBool.cpp
)

################################################
#   Library Setup
################################################

add_library(butane 
    ${NET_SOURCES}
    ${UTIL_SOURCES}
)

include(GNUInstallDirs)

target_include_directories(
    butane PUBLIC
    $<BUILD_INTERFACE:${NET_INCLUDE}>
    $<BUILD_INTERFACE:${UTIL_INCLUDE}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS butane
    EXPORT  butaneConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT butaneConfig
    NAMESPACE butane::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/butane
)

################################################
#   Install directories
################################################

install(
    DIRECTORY ${NET_INCLUDE}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/butane
)

install(
    DIRECTORY ${UTIL_INCLUDE}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/butane
)

################################################
#   Examples
################################################

if(COMPILE_EXAMPLES)

    message(">>> Building examples")

    add_subdirectory(net/examples)
    add_subdirectory(util/examples)

endif(COMPILE_EXAMPLES)

################################################ 
#   Tests
################################################

if(COMPILE_TESTS)

    message(">>> Building tests")

    add_subdirectory(net/tests)
    add_subdirectory(util/tests)

endif(COMPILE_TESTS)
