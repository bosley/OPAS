TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    sorts.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    sorts.h

