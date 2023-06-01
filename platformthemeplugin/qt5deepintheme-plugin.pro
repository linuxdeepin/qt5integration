QT       += dbus x11extras dtkgui
QT       += core-private gui-private
greaterThan(QT_MAJOR_VERSION, 4) {
  # Qt >= 5.8
  greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): QT += theme_support-private
  else: QT += platformsupport-private
}

TARGET = qdeepin
TEMPLATE = lib
CONFIG += plugin link_pkgconfig c++11

# Test using dthemed_icon_lookup
load(configure)
qtCompileTest(dthemed_icon_lookup) {
    DEFINES += DTHEMED_ICON_LOOKUP
    PKGCONFIG += dtkwidget til

    SOURCES += diconengine.cpp
    HEADERS += diconengine.h
}

PKGCONFIG += mtdev Qt5Xdg x11

PKG_CONFIG = $$pkgConfigExecutable()
XDG_ICON_VERSION = $$system($$PKG_CONFIG --modversion Qt5XdgIconLoader)
XDG_ICON_VERSION_LIST = $$split(XDG_ICON_VERSION, .)
INCLUDEPATH += $$system($$PKG_CONFIG --variable includedir Qt5XdgIconLoader)/qt5xdgiconloader/$$XDG_ICON_VERSION
DEFINES += "XDG_ICON_VERSION_MAR=$$first(XDG_ICON_VERSION_LIST)"

PLUGIN_TYPE = platformthemes
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QDeepinTheme

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/platformthemes

SOURCES += qdeepintheme.cpp \
    main.cpp \
    qdeepinfiledialoghelper.cpp \
    dthemesettings.cpp \
    $$_PRO_FILE_PWD_/../3rdparty/qdbustrayicon.cpp \
    $$_PRO_FILE_PWD_/../3rdparty/qstatusnotifieritemadaptor.cpp \
    $$_PRO_FILE_PWD_/../3rdparty/qdbusmenuconnection.cpp

HEADERS += qdeepintheme.h \
    qdeepinfiledialoghelper.h \
    dthemesettings.h \
    $$_PRO_FILE_PWD_/../3rdparty/qdbustrayicon_p.h \
    $$_PRO_FILE_PWD_/../3rdparty/qstatusnotifieritemadaptor_p.h \
    $$_PRO_FILE_PWD_/../3rdparty/qdbusmenuconnection_p.h

DISTFILES += \
    deepin.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/platformthemes
    INSTALLS += target
}

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

#dbus_interface_filedialogmanager = /usr/share/dbus-1/interfaces/com.deepin.filemanager.filedialogmanager.xml
#dbus_interface_filedialog = /usr/share/dbus-1/interfaces/com.deepin.filemanager.filedialog.xml

#!exists($$dbus_interface_filedialogmanager) {
#    warning(Not found $$dbus_interface_filedialogmanager)
    dbus_interface_filedialogmanager = $$PWD/xmls/com.deepin.filemanager.filedialogmanager.xml
#}

#message(Will be use $$dbus_interface_filedialogmanager)

#!exists($$dbus_interface_filedialog) {
#    warning(Not found $$dbus_interface_filedialog)
    dbus_interface_filedialog = $$PWD/xmls/com.deepin.filemanager.filedialog.xml
#}

#message(Will be use $$dbus_interface_filedialog)

DBUS_INTERFACES += $$dbus_interface_filedialogmanager $$dbus_interface_filedialog

RESOURCES += \
    icons/deepin-theme-plugin-icons.qrc \
    deepin-theme-plugin.qrc
