#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T17:21:21
#
#-------------------------------------------------

QT       += core gui widgets widgets-private

TARGET = dstyleplugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += dstyleplugin.cpp \ 
    style.cpp \ 
    sliderhelper.cpp \
    colorutils.cpp \
    geometryutils.cpp \
    paletteextended.cpp \
    pushbuttonhelper.cpp \
    framehelper.cpp \
    lineedithelper.cpp \
    commonhelper.cpp

HEADERS += dstyleplugin.h \
    style.h \
    common.h \
    sliderhelper.h \
    colorutils.h \
    geometryutils.h \
    paletteextended.h \
    pushbuttonhelper.h \
    framehelper.h \
    lineedithelper.h \
    commonhelper.h

DISTFILES += dstyleplugin.json

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/styles/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}

RESOURCES += \
    colorschemes.qrc
