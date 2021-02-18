TEMPLATE = app
QT += dtkgui concurrent dtkwidget
CONFIG -= app_bundle

unix:LIBS += -lgtest

QMAKE_CXXFLAGS += -fno-access-control
QMAKE_LFLAGS += -fno-access-control

include($$PWD/iconengines/iconengines.pri)
include($$PWD/styleplugins/styleplugins.pri)

SOURCES += \
    $$PWD/main.cpp

load(dtk_testcase)
