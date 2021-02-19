QMAKE_RPATHDIR += $$PWD/../../../bin/plugins/styles
unix:LIBS += -L$$PWD/../../../bin/plugins/styles -lchameleon

INCLUDEPATH += $$PWD/../../../styleplugins/chameleon

SOURCES += \
    $$PWD/ut_chameleonstyle.cpp \
