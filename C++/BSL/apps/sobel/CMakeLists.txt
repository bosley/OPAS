cmake_minimum_required(VERSION 3.8)

# This is where the opencv_install.sh script will place the installation
SET(OpenCV_DIR $ENV{HOME}/OpenCV/installation/OpenCV-master/lib/cmake/opencv4)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(BslCore REQUIRED)
include_directories(${BSLCORE_INCLUDE_DIRS})

find_package(BslCv REQUIRED)
include_directories(${BSLCV_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
)

add_executable(sobelApp
        main.cpp
)

target_link_libraries(sobelApp
    PRIVATE 
        ${OpenCV_LIBS}
        ${BSLCORE_LIBRARIES}
        ${BSLCV_LIBRARIES}
)