#-------------------------------------------------
#
# Project created by QtCreator 2019-01-07T16:25:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = grove
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += /usr/local/Cellar/opencv/3.4.3/include \

LIBS += -L/usr/local/Cellar/opencv/3.4.3/lib \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui\
     -lopencv_video\
     -lopencv_videoio

FORMS += \
    gui/MainWindow.ui \
    gui/video/GroveMatDisplay.ui

HEADERS += \
    gui/video/CVMatDisplayDriver.h \
    gui/widgets/video/WidgetVideoDisplay.h \
    gui/MainWindow.h \
    modules/bus/Bus.h \
    modules/cli/cli.h \
    modules/vision/Camera.h \
    modules/vision/Processor.h \
    modules/types.h \
    gui/video/GroveMatDisplay.h \
    modules/vision/Recorder.h \
    modules/vision/Vision.h \
    modules/system/HardwareConfig.h \
    algorithms/sobel/Sobel.h \
    modules/vision/processes/ProcessIF.h \
    modules/vision/processes/BackgroundRemoval.h

SOURCES += \
    gui/video/CVMatDisplayDriver.cpp \
    gui/widgets/video/WidgetVideoDisplay.cpp \
    gui/MainWindow.cpp \
    modules/cli/cli.cpp \
    modules/vision/Camera.cpp \
    modules/vision/Processor.cpp \
    main.cpp \
    gui/video/GroveMatDisplay.cpp \
    modules/vision/Recorder.cpp \
    modules/vision/Vision.cpp \
    algorithms/sobel/Sobel.cpp \
    modules/vision/processes/BackgroundRemoval.cpp
