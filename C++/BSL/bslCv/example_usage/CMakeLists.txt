cmake_minimum_required(VERSION 3.8)

project(BslCvExampleUsage)

SET(OpenCV_DIR $ENV{HOME}/OpenCV/installation/OpenCV-master/lib/cmake/opencv4)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(BslCv REQUIRED)
include_directories(${BSLCV_INCLUDE_DIRS})

# build type
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
)

# add binary
add_executable(BslCvExampleUsage main.cpp)

# add library
target_link_libraries(BslCvExampleUsage ${BSLCV_LIBRARIES})
target_link_libraries(BslCvExampleUsage ${OpenCV_LIBS})
