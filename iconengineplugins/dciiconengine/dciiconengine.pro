TARGET  = dtkdciicon
TEMPLATE = lib
CONFIG += plugin

HEADERS += dciiconengine.h
SOURCES += main.cpp \
           dciiconengine.cpp
OTHER_FILES += dciiconengine.json

QT += gui-private dtkgui
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/$${BASED_DTK_DIR}
    INSTALLS += target
}
