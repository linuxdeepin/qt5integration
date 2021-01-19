TEMPLATE = app
QT += dtkgui
CONFIG -= app_bundle

CONFIG += testcase no_testcase_installs

unix:LIBS += -lgtest

QMAKE_CXXFLAGS += -fno-access-control
QMAKE_LFLAGS += -fno-access-control

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -g -Wall  -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0
}

include($$PWD/iconengines/iconengines.pri)

SOURCES += \
    $$PWD/main.cpp
