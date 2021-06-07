cmake_minimum_required(VERSION 3.5)

project (Devik_Lang VERSION 0.0.0 LANGUAGES CXX)

option(TESTS    "Enable Tests"    ON)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/devik/Devik.cpp
        ${PROJECT_SOURCE_DIR}/memory/Memory.cpp
        ${PROJECT_SOURCE_DIR}/lang/Ast.cpp
        ${PROJECT_SOURCE_DIR}/lang/Keywords.cpp
        ${PROJECT_SOURCE_DIR}/lang/Lexer.cpp
        ${PROJECT_SOURCE_DIR}/lang/Parser.cpp
        ${PROJECT_SOURCE_DIR}/lang/Token.cpp
        ${PROJECT_SOURCE_DIR}/lang/Page.cpp
        ${PROJECT_SOURCE_DIR}/lang/SourceTree.cpp
        )

if(TESTS)
    include(${PROJECT_SOURCE_DIR}/cmake/FindCppuTest.cmake)
    add_subdirectory(memory/tests)
endif()

add_executable(devik
        ${PROJECT_SOURCE_DIR}/cli/main.cpp
        ${SOURCES}
        )

target_include_directories(devik PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

target_compile_options(devik PRIVATE -Werror)
target_compile_features(devik PRIVATE cxx_std_17)


