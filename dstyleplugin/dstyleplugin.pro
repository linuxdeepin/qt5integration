#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T17:21:21
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = dstyleplugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += dstyleplugin.cpp \ 
    style.cpp \ 
    sliderhelper.cpp \
    colorutils.cpp \
    geometryutils.cpp

HEADERS += dstyleplugin.h \
    style.h \
    common.h \
    sliderhelper.h \
    colorutils.h \
    geometryutils.h

DISTFILES += dstyleplugin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}
