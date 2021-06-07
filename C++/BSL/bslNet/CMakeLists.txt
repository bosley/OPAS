cmake_minimum_required(VERSION 3.8)

project(BslNet)

##################################################
# Find and include BslCore
##################################################

find_package(BslCore REQUIRED)
include_directories(${BSLCORE_INCLUDE_DIRS})

##################################################
# Find and include json
##################################################

find_package(nlohmann_json 3.7.3 REQUIRED)

##################################################
# Options
##################################################

option(COMPILE_EXAMPLES "Compile the examples" ON)
option(COMPILE_TESTS    "Compile the tests"    ON)
option(BUILD_THREADED   "Compile with threads" ON)

##################################################
# Find CPPU test, and set project env
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

set(CMAKE_CXX_STANDARD 17)

if(BUILD_THREADED)

    add_compile_definitions(BSL_NET_COMPILE_WITH_THREADS)

    if (MSVC)
      set(LIBS ${LIBS} ws2_32.lib)
    else()
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
    endif(MSVC)
endif()

##################################################
# Modules
##################################################

set(MODULES
        net
)

##################################################
# Add Threaded Modules
##################################################

if(BUILD_THREADED)
    list(APPEND MODULES
            serv)
endif()

##################################################
# Glob modules
##################################################

foreach(i ${MODULES})

    file(GLOB CURRENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${i}/*.cpp)
    file(GLOB CURRENT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${i}/*.hpp)

    list (APPEND SOURCES ${CURRENT_SOURCES})
    list (APPEND HEADERS ${CURRENT_HEADERS})

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${i})
    
endforeach()

##################################################
# Configure Library
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)

##################################################
# Tests and Examples 
##################################################

foreach(i ${MODULES})

    if(COMPILE_EXAMPLES)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${i}/examples)
    endif()

    if(COMPILE_TESTS)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${i}/tests)
    endif()

endforeach()

##################################################
# Configure Install
##################################################

include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)


