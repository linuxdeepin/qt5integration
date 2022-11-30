TARGET  = dci
TEMPLATE = lib
CONFIG += plugin

HEADERS += \
    qdciiohandler.h
SOURCES += main.cpp \
    qdciiohandler.cpp
OTHER_FILES += dci.json

QT += dtkgui

DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/imageformats/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/imageformats/$${BASED_DTK_DIR}
    mimetype.files += $$PWD/image-dci.xml

    isEmpty(MIME_TYPE_FILE_INSTALL_PATH): mimetype.path = /usr/share/mime/packages
    else: mimetype.path = $${MIME_TYPE_FILE_INSTALL_PATH}
    message(Insting $${mimetype.files} to $${mimetype.path});

    INSTALLS += target mimetype
}
