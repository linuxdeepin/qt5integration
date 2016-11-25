#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T17:21:21
#
#-------------------------------------------------

QT        += core gui widgets widgets-private
CONFIG    += link_pkgconfig
PKGCONFIG += dtkbase dtkwidget

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
    commonhelper.cpp \
    scrollbarhelper.cpp \
    painterhelper.cpp \
    menuhelper.cpp

HEADERS += dstyleplugin.h \
    style.h \
    common.h \
    colorutils.h \
    geometryutils.h \
    paletteextended.h \
    framehelper.h \
    lineedithelper.h \
    commonhelper.h \
    painterhelper.h

DISTFILES += dstyleplugin.json

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/styles/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}

RESOURCES += \
    brushchemes.qrc \
    assets.qrc
