TARGET = dxcb
CONFIG += link_pkgconfig

PKGCONFIG += x11-xcb xi xcb-renderutil sm ice xcb-render dbus-1 xcb\
             xcb-image xcb-icccm xcb-sync xcb-xfixes xcb-shm xcb-randr\
             xcb-shape xcb-keysyms xcb-xkb xkbcommon-x11 mtdev egl

greaterThan(QT_MINOR_VERSION, 5): PKGCONFIG += xcb-xinerama

#LIBS += -ldl

greaterThan(QT_MINOR_VERSION, 4): LIBS += -lQt5XcbQpa

HEADERS += \
    $$PWD/windoweventhook.h \
    $$PWD/xcbnativeeventfilter.h \

SOURCES += \
    $$PWD/windoweventhook.cpp \
    $$PWD/xcbnativeeventfilter.cpp \
    $$PWD/utility_x11.cpp

INCLUDEPATH += $$PWD/libqt5xcbqpa-dev

# to support custom cursors with depth > 1
contains(QT_CONFIG, xcb-render) {
    DEFINES += XCB_USE_RENDER
}

# build with session management support
contains(QT_CONFIG, xcb-sm) {
    DEFINES += XCB_USE_SM
}

contains(QT_CONFIG, xcb-qt) {
    DEFINES += XCB_USE_RENDER
}

exists($$PWD/libqt5xcbqpa-dev) {
    !system(cd $$PWD/libqt5xcbqpa-dev && git checkout $$VERSION) {
        !system(cd $$PWD/libqt5xcbqpa-dev && git fetch -p):error(update libqt5xcbqpa header sources failed)
        !system(cd $$PWD/libqt5xcbqpa-dev && git checkout $$VERSION):error(Not support Qt Version: $$VERSION)
    }
} else {
    !system(git clone https://github.com/zccrs/libqt5xcbqpa-dev.git):error(clone libqt5xcbqpa header sources failed)
    !system(cd $$PWD/libqt5xcbqpa-dev && git checkout $$VERSION):error(Not support Qt Version: $$VERSION)
}
