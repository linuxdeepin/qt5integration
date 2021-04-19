TARGET  = dsvg$${DTK_VERSION}
TEMPLATE = lib
CONFIG += plugin

HEADERS += qsvgiohandler.h
SOURCES += main.cpp \
           qsvgiohandler.cpp
OTHER_FILES += svg.json

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkgui$${DTK_VERSION}

DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/imageformats/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/imageformats/$${BASED_DTK_DIR}
    INSTALLS += target
}
