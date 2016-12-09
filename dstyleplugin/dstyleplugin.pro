#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T17:21:21
#
#-------------------------------------------------

QT        += core gui widgets widgets-private
CONFIG    += link_pkgconfig
PKGCONFIG += dtkbase dtkwidget dtkutil

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
    menuhelper.cpp \
    dstyleanimation.cpp \
    progressbarhelper.cpp \
    checkboxhelper.cpp

HEADERS += dstyleplugin.h \
    style.h \
    common.h \
    colorutils.h \
    geometryutils.h \
    paletteextended.h \
    commonhelper.h \
    painterhelper.h \
    dstyleanimation.h

DISTFILES += dstyleplugin.json

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/styles/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}

RESOURCES += \
    brushchemes.qrc \
    assets_light.qrc \
    assets_dark.qrc \
    assets.qrc
