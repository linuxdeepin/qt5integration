CONFIG(debug, debug|release) {
    DEFINES += NDEBUG
}

INCLUDEPATH += $$PWD/../../../iconengineplugins/svgiconengine

SOURCES += \
    $$PWD/ut_qsvgiconengine.cpp

RESOURCES += \
    $$PWD/icons.qrc
