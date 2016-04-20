#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T08:25:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lqwd
TEMPLATE = app


SOURCES += main.cpp\
        hq.cpp \
    widgets/console/bkons.cpp \
    lib/coremodule.cpp \
    widgets/console/consolesettings.cpp \
    widgets/console/errormachine.cpp \
    lib/coreprocess.cpp \
    lib/corenexus.cpp \
    widgets/console/messagebox.cpp

HEADERS  += hq.h \
    widgets/console/bkons.h \
    lib/defines.h \
    lib/coremodule.h \
    widgets/console/consolesettings.h \
    widgets/console/defines.h \
    widgets/console/errormachine.h \
    lib/coreprocess.h \
    lib/corenexus.h \
    widgets/console/messagebox.h \
    lib/commandStructures.h

FORMS    += hq.ui \
    widgets/console/messagebox.ui

RESOURCES += \
    config.qrc \
    icons/icons.qrc

OTHER_FILES +=
