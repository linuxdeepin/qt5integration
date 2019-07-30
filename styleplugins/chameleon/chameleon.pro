QT        += core gui widgets widgets-private core-private gui-private dtkgui dtkwidget

TARGET = chameleon
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    $$PWD/main.cpp \
    chameleonstyle.cpp

DISTFILES += $${TARGET}.json

DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/styles/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/
    INSTALLS += target
}

HEADERS += \
    chameleonstyle.h
