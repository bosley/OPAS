cmake_minimum_required(VERSION 3.0.0)

#
#   Create the Application for the DTC
#
project(app_dtc CXX)

#
#   Executable
#
add_executable(app_dtc main.cpp)

#
#   Include the DTC lib path
#
target_include_directories(app_dtc PRIVATE ${LIB_DTC_PATH})

#
#   Link the DTC lib
#
target_link_libraries(app_dtc PRIVATE dtc)

#
#   Link the Arduino stuff
#
target_link_arduino_libraries(app_dtc PRIVATE core)

#
#   Create an uploadable target
#
target_enable_arduino_upload(app_dtc)