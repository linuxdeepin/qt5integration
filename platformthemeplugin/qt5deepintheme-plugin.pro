#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:24:29
#
#-------------------------------------------------

QT       += dbus x11extras
QT       += core-private gui-private platformsupport-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets widgets-private

TARGET = qdeepin
TEMPLATE = lib
CONFIG += plugin link_pkgconfig c++11

PKGCONFIG += mtdev dtkbase dtkwidget Qt5Xdg gtk+-2.0 x11

PLUGIN_TYPE = platformthemes
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QDeepinTheme

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/platformthemes

SOURCES += qdeepintheme.cpp \
    main.cpp \
    qdeepinfiledialoghelper.cpp \
    diconproxyengine.cpp

HEADERS += qdeepintheme.h \
    qdeepinfiledialoghelper.h \
    diconproxyengine.h

DISTFILES += \
    deepin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/platformthemes
    INSTALLS += target
}

RESOURCES += \
    theme.qrc

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

dbus_interface_filedialogmanager = /usr/share/dbus-1/interfaces/com.deepin.filemanager.filedialogmanager.xml
dbus_interface_filedialog = /usr/share/dbus-1/interfaces/com.deepin.filemanager.filedialog.xml

!exists($$dbus_interface_filedialogmanager) {
    warning(Not found $$dbus_interface_filedialogmanager)
    dbus_interface_filedialogmanager = $$PWD/xmls/com.deepin.filemanager.filedialogmanager.xml
}

message(Will be use $$dbus_interface_filedialogmanager)

!exists($$dbus_interface_filedialog) {
    warning(Not found $$dbus_interface_filedialog)
    dbus_interface_filedialog = $$PWD/xmls/com.deepin.filemanager.filedialog.xml
}

message(Will be use $$dbus_interface_filedialog)

DBUS_INTERFACES += $$dbus_interface_filedialogmanager $$dbus_interface_filedialog
