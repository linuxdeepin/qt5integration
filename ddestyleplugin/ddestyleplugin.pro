#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T17:21:21
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ddestyleplugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += ddestyleplugin.cpp \
    darkstyle.cpp \
    lightstyle.cpp

HEADERS += ddestyleplugin.h \
    darkstyle.h \
    lightstyle.h

DISTFILES += ddestyleplugin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}
