cmake_minimum_required(VERSION 3.7)

project(CruxBus)

add_compile_options(-std=c++11 -Wall -Werror -pthread)

######################################
# Setup project variables
######################################

set (cruxBus_VERSION_MAJOR 0)
set (cruxBus_VERSION_MINOR 1)
set (cruxBus_VERSION_PATCH 0)
set (cruxBus_VERSION "${cruxBus_VERSION_MAJOR}.${cruxBus_VERSION_MINOR}.${cruxBus_VERSION_PATCH}")

set(APP_NAME        cruxBus)
set(APP_LIB_NAME    "cruxBusLib-${cruxBus_VERSION}")
set(APP_LIB_SOURCES
    CBBus.cpp
    implementations/tsgp/TSGPBusImpl.cpp
)

######################################
# Compile options
######################################
option(COMPILE_TESTS "Compile the tests" OFF)
option(COMPILE_EXAMPLES "Compile the examples" OFF)

######################################
# Setup library - SHARED
######################################
add_library(${APP_LIB_NAME} SHARED ${APP_LIB_SOURCES})

######################################
# Create TARGET
######################################
target_include_directories(${APP_LIB_NAME} 
    PUBLIC
      ${PROJECT_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}/implementations/tsgp
)

######################################
# Setup install information
######################################
install(TARGETS ${APP_LIB_NAME}
    LIBRARY DESTINATION lib
)

######################################
# Testing check
######################################
if(COMPILE_TESTS)
    add_subdirectory(tests)
endif(COMPILE_TESTS)

if(COMPILE_EXAMPLES)
    add_subdirectory(examples)
endif(COMPILE_EXAMPLES)
