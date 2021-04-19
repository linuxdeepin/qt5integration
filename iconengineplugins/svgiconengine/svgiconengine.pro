TARGET  = dsvgicon$${DTK_VERSION}
TEMPLATE = lib
CONFIG += plugin

HEADERS += qsvgiconengine.h
SOURCES += main.cpp \
           qsvgiconengine.cpp
OTHER_FILES += qsvgiconengine.json

CONFIG += c++11

QT += gui-private dtkgui$${DTK_VERSION} concurrent
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/$${BASED_DTK_DIR}
    INSTALLS += target
}
