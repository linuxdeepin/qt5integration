TARGET  = dtkbuiltin$${DTK_VERSION}
TEMPLATE = lib
CONFIG += plugin

HEADERS += dbuiltiniconengine.h
SOURCES += main.cpp \
           dbuiltiniconengine.cpp
OTHER_FILES += builtinengine.json

QT += gui-private dtkgui$${DTK_VERSION}
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/$${BASED_DTK_DIR}
    INSTALLS += target
}
