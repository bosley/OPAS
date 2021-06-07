cmake_minimum_required(VERSION 3.5)

project (keyvis)

set(CMAKE_CXX_STANDARD 17)

option(TESTS    "Enable Tests"    ON)

set(KEYVIS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Engine.cpp
)

if(TESTS)
    add_subdirectory(tests)
endif()

add_executable(keyvis 
    ${CMAKE_SOURCE_DIR}/src/main.cpp 
    ${KEYVIS_SOURCES}
)

target_include_directories(keyvis
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)


