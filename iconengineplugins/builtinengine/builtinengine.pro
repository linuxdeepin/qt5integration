TARGET  = dtkbuiltin
TEMPLATE = lib
CONFIG += plugin

HEADERS += dbuiltiniconengine.h
SOURCES += main.cpp \
           dbuiltiniconengine.cpp
OTHER_FILES += builtinengine.json

QT += gui-private dtkgui
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/
    INSTALLS += target
}
