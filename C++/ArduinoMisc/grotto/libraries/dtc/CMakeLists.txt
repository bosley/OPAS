cmake_minimum_required(VERSION 3.0.0)

project(dtc CXX)


set(LIB_DTC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/DtcServer.hpp
)

set(LIB_DTC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/DtcServer.cpp
)

add_library(dtc ${LIB_DTC_HEADERS} ${LIB_DTC_SOURCES})

target_link_arduino_libraries(dtc
	AUTO_INTERFACE ${LIB_DTC_HEADERS}
	AUTO_PRIVATE
)
