TEMPLATE = app
QT += dtkgui concurrent
CONFIG -= app_bundle

unix:LIBS += -lgtest

QMAKE_CXXFLAGS += -fno-access-control
QMAKE_LFLAGS += -fno-access-control

include($$PWD/iconengines/iconengines.pri)

SOURCES += \
    $$PWD/main.cpp

load(dtk_testcase)
