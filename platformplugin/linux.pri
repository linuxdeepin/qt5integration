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
    $$PWD/dxcbwmsupport.h

SOURCES += \
    $$PWD/windoweventhook.cpp \
    $$PWD/xcbnativeeventfilter.cpp \
    $$PWD/utility_x11.cpp \
    $$PWD/dxcbwmsupport.cpp \
    $$PWD/dforeignplatformwindow_x11.cpp

contains(QT_CONFIG, xcb-xlib)|qtConfig(xcb-xlib) {
    DEFINES += XCB_USE_XLIB
    QMAKE_USE += xcb_xlib

    contains(QT_CONFIG, xinput2)|qtConfig(xinput2) {
        DEFINES += XCB_USE_XINPUT2
        QMAKE_USE += xinput2

        !isEmpty(QMAKE_LIBXI_VERSION_MAJOR) {
            DEFINES += LIBXI_MAJOR=$$QMAKE_LIBXI_VERSION_MAJOR \
                       LIBXI_MINOR=$$QMAKE_LIBXI_VERSION_MINOR \
                       LIBXI_PATCH=$$QMAKE_LIBXI_VERSION_PATCH
        }
    }
}

# build with session management support
contains(QT_CONFIG, xcb-sm)|qtConfig(xcb-sm) {
    DEFINES += XCB_USE_SM
    QMAKE_USE += x11sm
}

!contains(QT_CONFIG, system-xcb)|qtConfig(system-xcb) {
    DEFINES += XCB_USE_RENDER
    QMAKE_USE += xcb
} else {
    LIBS += -lxcb-xinerama  ### there is no configure test for this!
    contains(QT_CONFIG, xkb)|qtConfig(xkb): QMAKE_USE += xcb_xkb
    # to support custom cursors with depth > 1
    contains(QT_CONFIG, xcb-render)|qtConfig(xkb) {
        DEFINES += XCB_USE_RENDER
        QMAKE_USE += xcb_render
    }
    QMAKE_USE += xcb_syslibs
}

contains(QT_CONFIG, xcb-qt) {
    DEFINES += XCB_USE_RENDER
}

exists($$PWD/libqt5xcbqpa-dev/$$VERSION) {
    INCLUDEPATH += $$PWD/libqt5xcbqpa-dev/$$VERSION
} else {
    error(Not support Qt Version: $$VERSION)
}
