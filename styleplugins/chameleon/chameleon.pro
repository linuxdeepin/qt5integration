QT        += core gui widgets widgets-private core-private gui-private dtkgui$${DTK_VERSION} dtkwidget$${DTK_VERSION}

TARGET = chameleon$${DTK_VERSION}
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/chameleonstyle.cpp \
    $$PWD/../dstyleplugin/dstyleanimation.cpp

DISTFILES += $${TARGET}.json

DESTDIR = $$_PRO_FILE_PWD_/../../bin/plugins/styles/$${BASED_DTK_DIR}

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/styles/$${BASED_DTK_DIR}
    INSTALLS += target
}

HEADERS += \
    $$PWD/common.h \
    $$PWD/chameleonstyle.h \
    $$PWD/../dstyleplugin/dstyleanimation.h

RESOURCES += \
    resources.qrc
