TEMPLATE = app
QT += dtkgui$${DTK_VERSION} concurrent dtkwidget$${DTK_VERSION}
CONFIG -= app_bundle

unix:LIBS += -lgtest

QMAKE_CXXFLAGS += -fno-access-control
QMAKE_LFLAGS += -fno-access-control

CONFIG(debug, debug|release) {
LIBS += -lgtest -lgmock
QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -fsanitize=address -O2
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -fsanitize=address -O2
MAKE_CXX += -g -fprofile-arcs -ftest-coverage -fsanitize=address -O2
}

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
