#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T19:46:44
#
#-------------------------------------------------

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = DXcbIntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/platforms

QT       += opengl x11extras
QT       += core-private platformsupport-private #xcb_qpa_lib-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets widgets-private

TEMPLATE = lib
VERSION = $$QT_VERSION

linux: include($$PWD/linux.pri)
windows: include($$PWD/windows.pri)

CONFIG += plugin c++11

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/dplatformintegration.cpp \
    $$PWD/dplatformbackingstore.cpp \
    $$PWD/vtablehook.cpp \
    $$PWD/dplatformwindowhook.cpp \
    $$PWD/dplatformnativeinterface.cpp

HEADERS += \
    $$PWD/dplatformintegration.h \
    $$PWD/dplatformbackingstore.h \
    $$PWD/vtablehook.h \
    $$PWD/dplatformwindowhook.h \
    $$PWD/utility.h \
    $$PWD/global.h \
    $$PWD/dplatformnativeinterface.h

DISTFILES += \
    $$PWD/dpp.json

target.path = $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

contains(QT_CONFIG, xcb-xlib) {
    DEFINES += XCB_USE_XLIB

    contains(QT_CONFIG, xinput2) {
        DEFINES += XCB_USE_XINPUT2
        !isEmpty(QMAKE_LIBXI_VERSION_MAJOR) {
            DEFINES += LIBXI_MAJOR=$$QMAKE_LIBXI_VERSION_MAJOR \
                       LIBXI_MINOR=$$QMAKE_LIBXI_VERSION_MINOR \
                       LIBXI_PATCH=$$QMAKE_LIBXI_VERSION_PATCH
        }
    }
}
