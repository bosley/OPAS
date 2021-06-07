cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/Arduino-CMake-Toolchain/Arduino-toolchain.cmake")

message(CMAKE_TOOLCHAIN_FILE ": ${CMAKE_TOOLCHAIN_FILE}")

project(Grotto)

add_subdirectory(external)
add_subdirectory(libraries)
add_subdirectory(apps)