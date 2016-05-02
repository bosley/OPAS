TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    network.cpp \
    neuron.cpp \
    nnet.cpp

HEADERS += \
    network.h \
    neuron.h \
    nnet.h

