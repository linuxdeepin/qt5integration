TARGET  = dtkiconproxy
TEMPLATE = lib
CONFIG += plugin

HEADERS += diconproxyengine.h
SOURCES += main.cpp \
           diconproxyengine.cpp
OTHER_FILES += diconproxyengine.json

QT +=  dtkgui KIconThemes
DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/iconengines/

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/iconengines/
    INSTALLS += target
}
