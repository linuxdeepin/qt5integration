QMAKE_RPATHDIR += $$PWD/../../../bin/plugins/styles/$${BASED_DTK_DIR}
unix:LIBS += -L$$PWD/../../../bin/plugins/styles/$${BASED_DTK_DIR} -lchameleon

INCLUDEPATH += $$PWD/../../../styleplugins/chameleon

SOURCES += \
    $$PWD/ut_chameleonstyle.cpp \
