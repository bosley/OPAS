#-------------------------------------------------
#
# Project created by QtCreator 2016-01-30T05:40:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 434Term
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    framework/sockets/sockethandler.cpp \
    framework/engine/engine.cpp

HEADERS  += mainwindow.h \
    framework/sockets/sockethandler.h \
    framework/engine/engine.h

FORMS    += mainwindow.ui
