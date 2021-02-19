TEMPLATE = app
QT += dtkgui concurrent dtkwidget
CONFIG -= app_bundle

unix:LIBS += -lgtest

QMAKE_CXXFLAGS += -fno-access-control
QMAKE_LFLAGS += -fno-access-control

include($$PWD/iconengines/iconengines.pri)
include($$PWD/styleplugins/styleplugins.pri)

HEADERS += \
    $$PWD/../iconengineplugins/builtinengine/dbuiltiniconengine.h \
    $$PWD/../iconengineplugins/svgiconengine/qsvgiconengine.h \
    $$PWD/../imageformatplugins/svg/qsvgiohandler.h \
    $$PWD/../styleplugins/chameleon/chameleonstyle.h \
    $$PWD/../styleplugins/chameleon/common.h \

SOURCES += \
    $$PWD/../iconengineplugins/builtinengine/dbuiltiniconengine.cpp \
    $$PWD/../iconengineplugins/svgiconengine/qsvgiconengine.cpp \
    $$PWD/../imageformatplugins/svg/qsvgiohandler.cpp \
    $$PWD/../styleplugins/chameleon/chameleonstyle.cpp \


SOURCES += \
    $$PWD/main.cpp

load(dtk_testcase)
