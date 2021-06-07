cmake_minimum_required(VERSION 3.8)

project(obelusCli VERSION 0.0.1 LANGUAGES CXX)

find_package(butane REQUIRED)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++17")

add_executable(obelusCli 
        main.cpp
        ${COMMON_SOURCES}
)

target_link_libraries(obelusCli
    PUBLIC
        butane::butane
)
