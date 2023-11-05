# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus Widgets)
set(QT_LIBS Qt${QT_VERSION_MAJOR}::DBus Qt${QT_VERSION_MAJOR}::Widgets)
if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5X11Extras REQUIRED)
    list(APPEND QT_LIBS Qt5::X11Extras)
    if(QT_VERSION_MINOR GREATER_EQUAL 8) # Qt5.8
        find_package(Qt5ThemeSupport REQUIRED)
        list(APPEND QT_LIBS Qt5::ThemeSupportPrivate)
    else()
        list(APPEND QT_LIBS Qt5::PlatformSupportPrivate)
    endif()
else()
    list(APPEND QT_LIBS Qt6::GuiPrivate)
endif()

include(../filedialog/filedialog.cmake)

dtk_add_plugin(
    NAME
        qdeepinfiledialog
    OUTPUT_DIR
        ${PLUGIN_OUTPUT_BASE_DIR}/platformthemes
    INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/../filedialog
    SOURCES
        ${DFM_DLG_SOURCES}
        qdeepintheme.cpp
        main.cpp
        ${DBUS_INTERFACES}
    HEADERS
        ${DFM_DLG_HEADERS}
        qdeepintheme.h
    DEPENDENCIES
        ${QT_LIBS}
    DEFINITIONS
        ${DEFS}
)
