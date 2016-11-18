#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:24:29
#
#-------------------------------------------------

QT       += core-private gui-private platformsupport-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets widgets-private

TARGET = qdeepin
TEMPLATE = lib
CONFIG += plugin link_pkgconfig c++11

PKGCONFIG += mtdev dde-file-manager dtkbase dtkwidget Qt5Xdg gtk+-2.0

PLUGIN_TYPE = platformthemes
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QDeepinTheme

DESTDIR = $$_PRO_FILE_PWD_/../bin/plugins/platformthemes

SOURCES += qdeepintheme.cpp \
    main.cpp \
    qdeepinfiledialoghelper.cpp \
    qdeepinplatformmenu.cpp \
    qdeepinplatformmenuitem.cpp \
    diconproxyengine.cpp

HEADERS += qdeepintheme.h \
    qdeepinfiledialoghelper.h \
    qdeepinplatformmenu.h \
    qdeepinplatformmenuitem.h \
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
