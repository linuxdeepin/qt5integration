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
QT       += core-private #xcb_qpa_lib-private
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets widgets-private
  # Qt >= 5.8
  greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
  else: QT += platformsupport-private
}

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
    $$PWD/dplatformnativeinterface.cpp \
    $$PWD/dframewindow.cpp \
    $$PWD/dplatformwindowhelper.cpp \
    $$PWD/dplatformbackingstorehelper.cpp \
    $$PWD/dplatformopenglcontexthelper.cpp

HEADERS += \
    $$PWD/dplatformintegration.h \
    $$PWD/dplatformbackingstore.h \
    $$PWD/vtablehook.h \
    $$PWD/dplatformwindowhook.h \
    $$PWD/utility.h \
    $$PWD/global.h \
    $$PWD/dplatformnativeinterface.h \
    $$PWD/dforeignplatformwindow.h \
    $$PWD/dframewindow.h \
    $$PWD/dplatformwindowhelper.h \
    $$PWD/dplatformbackingstorehelper.h \
    $$PWD/dplatformopenglcontexthelper.h \
    $$PWD/dwmsupport.h

DISTFILES += \
    $$PWD/dpp.json

target.path = $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
} else {
#    DEFINES += USE_NEW_IMPLEMENTING
}

RESOURCES += \
    cursors/cursor.qrc
