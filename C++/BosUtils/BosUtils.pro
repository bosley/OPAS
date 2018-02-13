TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    FuzzyBoolean/fbool.cpp \
    FuzzySets/fset.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    DoublyLinkedList/jdll.h \
    FuzzyBoolean/fbool.h \
    FuzzySets/fset.h \
    Sorting/datasorter.h

