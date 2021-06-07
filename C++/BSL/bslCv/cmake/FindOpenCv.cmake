################################################
# OpenCV
################################################

# This is where the opencv_install.sh script will place the installation
SET(OpenCV_DIR $ENV{HOME}/OpenCV/installation/OpenCV-master/lib/cmake/opencv4)

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )