#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T09:01:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 434-Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    socket/sockethandler.cpp \
    engine/engine.cpp \
    engine/work/bsorts.cpp

HEADERS  += mainwindow.h \
    socket/sockethandler.h \
    engine/engine.h \
    engine/work/bsorts.h

FORMS    += mainwindow.ui
