QT       += gui-private core gui

TARGET = xdgicon
TEMPLATE = lib
CONFIG += plugin link_pkgconfig

DEFINES += QT_DEPRECATED_WARNINGS

PKGCONFIG += mtdev Qt5Xdg x11 Qt5XdgIconLoader

PKG_CONFIG = $$pkgConfigExecutable()
XDG_ICON_VERSION = $$system($$PKG_CONFIG --modversion Qt5XdgIconLoader)
XDG_ICON_VERSION_LIST = $$split(XDG_ICON_VERSION, .)
INCLUDEPATH += $$system($$PKG_CONFIG --variable includedir Qt5XdgIconLoader)/qt5xdgiconloader/$$XDG_ICON_VERSION
DEFINES += "XDG_ICON_VERSION_MAR=$$first(XDG_ICON_VERSION_LIST)"

SOURCES += \
        xdgiconproxyengine.cpp \
        main.cpp

HEADERS += \
        xdgiconproxyengine.h \

OTHER_FILES += \
    xdgproxyiconplugin.json

DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/$${BASED_DTK_DIR}
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/$${BASED_DTK_DIR}
    INSTALLS += target
}
