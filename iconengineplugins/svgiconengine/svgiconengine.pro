TARGET  = dsvgicon
TEMPLATE = lib
CONFIG += plugin

HEADERS += qsvgiconengine.h
SOURCES += main.cpp \
           qsvgiconengine.cpp
OTHER_FILES += qsvgiconengine.json

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

QT += gui-private
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/
    INSTALLS += target
}
