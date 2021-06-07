cmake_minimum_required(VERSION 3.8)

find_package(BslCore REQUIRED)
include_directories(${BSLCORE_INCLUDE_DIRS})

find_package(BslNet REQUIRED)
include_directories(${BSNET_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(LIBS ${LIBS} ws2_32.lib)
else()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
endif(MSVC)

set(APP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Destination.cpp)

# Create app 1

add_executable(application1
        app1.cpp
)

target_link_libraries(application1
    PRIVATE 
        ${APP_SOURCES}
        ${BSLCORE_LIBRARIES}
        ${BSLNET_LIBRARIES}
)

# Create app 2

add_executable(application2
        app2.cpp
        )

target_link_libraries(application2
        PRIVATE
        ${APP_SOURCES}
        ${BSLCORE_LIBRARIES}
        ${BSLNET_LIBRARIES}
        )