QMAKE_RPATHDIR += $$PWD/../../../bin/plugins/styles/$${BASED_DTK_DIR}
unix:LIBS += -L$$PWD/../../../bin/plugins/styles/$${BASED_DTK_DIR} -lchameleon$${DTK_VERSION}

INCLUDEPATH += $$PWD/../../../styleplugins/chameleon

!contains(QMAKE_HOST.arch, mips.*) {
SOURCES += \
    $$PWD/ut_chameleonstyle.cpp \
}
