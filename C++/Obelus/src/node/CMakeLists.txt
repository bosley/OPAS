cmake_minimum_required(VERSION 3.8)

project(obelusNode VERSION 0.0.1 LANGUAGES CXX)

find_package(butane REQUIRED)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++17")

add_executable(obelusNode 
        main.cpp
        ${COMMON_SOURCES}
        lua.cpp
        hpic.cpp
)

target_link_libraries(obelusNode
    PUBLIC
        butane::butane
    PRIVATE
         lua5.3
)
