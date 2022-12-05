QMAKE_RPATHDIR += $$PWD/../../../bin/plugins/styles
unix:LIBS += -L$$PWD/../../../bin/plugins/styles -lchameleon

INCLUDEPATH += $$PWD/../../../styleplugins/chameleon

!contains(QMAKE_HOST.arch, mips.*) {
SOURCES += \
    $$PWD/ut_chameleonstyle.cpp \
}
