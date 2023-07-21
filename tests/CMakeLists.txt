# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5X11Extras REQUIRED)
    list(APPEND QT_LIBS Qt5::X11Extras)
    if(QT_VERSION_MINOR GREATER_EQUAL 8) # Qt5.8
        find_package(Qt5ThemeSupport REQUIRED)
        list(APPEND THEME_LIBS Qt5::ThemeSupportPrivate)
    else()
        list(APPEND THEME_LIBS Qt5::PlatformSupportPrivate) # TODO to be verified
    endif()
    set(3RD_PARTY_SOURCES
        ${CMAKE_SOURCE_DIR}/3rdparty/qdbustrayicon.cpp
        ${CMAKE_SOURCE_DIR}/3rdparty/qstatusnotifieritemadaptor.cpp
        ${CMAKE_SOURCE_DIR}/3rdparty/qdbusmenuconnection.cpp
    )
    set(3RD_PARTY_HEADERS
        ${CMAKE_SOURCE_DIR}/3rdparty/qdbustrayicon_p.h
        ${CMAKE_SOURCE_DIR}/3rdparty/qstatusnotifieritemadaptor_p.h
        ${CMAKE_SOURCE_DIR}/3rdparty/qdbusmenuconnection_p.h
    )
    qt5_add_dbus_interface(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/platformthemeplugin/xmls/com.deepin.filemanager.filedialog.xml filedialog_interface)
    qt5_add_dbus_interface(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/platformthemeplugin/xmls/com.deepin.filemanager.filedialogmanager.xml filedialogmanager_interface)
else()
    qt6_add_dbus_interface(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/platformthemeplugin/xmls/com.deepin.filemanager.filedialog.xml filedialog_interface)
    qt6_add_dbus_interface(DBUS_INTERFACES ${CMAKE_SOURCE_DIR}/platformthemeplugin/xmls/com.deepin.filemanager.filedialogmanager.xml filedialogmanager_interface)
endif()
add_executable(unit-tests
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.h
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.h
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci/qdciiohandler.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/common.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/dstyleanimation.h
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/dthemesettings.h
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/qdeepintheme.h
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/qdeepinfiledialoghelper.h
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.cpp
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.cpp
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci/qdciiohandler.cpp
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.cpp
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/dstyleanimation.cpp
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/dthemesettings.cpp
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/qdeepintheme.cpp
    ${CMAKE_SOURCE_DIR}/platformthemeplugin/qdeepinfiledialoghelper.cpp
    iconengines/svgiconengine/ut_qsvgiconengine.cpp
    iconengines/svgiconengine/icons.qrc
    imageformats/dci/ut_qdciiohandler.cpp
    imageformats/dci/dci_test_files.qrc
    imageformats/svg/ut_qsvgiohandler.cpp
    styleplugins/chameleon/ut_chameleonstyle.cpp
    styleplugins/chameleon/icons/theme-icons.qrc
    platformtheme/ut_dthemesettings.cpp
    platformtheme/ut_qdeepintheme.cpp
    platformtheme/ut_qdeepinfiledialoghelper.cpp
    platformtheme/test-files.qrc
    platformtheme/filedialogservice.h
    platformtheme/filedialogservice.cpp
    platformtheme/filedialogmanagerservice.h
    platformtheme/filedialogmanagerservice.cpp
    ut_pluginloading.cpp
    main.cpp
    ${3RD_PARTY_SOURCES}
    ${3RD_PARTY_HEADERS}
    ${DBUS_INTERFACES}
)
find_package(GTest REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Concurrent REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)
target_compile_options(unit-tests PRIVATE
    -g
    -fno-access-control
    -Wall
    -fprofile-arcs
    -ftest-coverage
    -fsanitize=address
    -fsanitize-recover=address
    -O0
)
target_link_options(unit-tests PRIVATE
    -fsanitize=address
    -fsanitize-recover=address
)
target_link_libraries(unit-tests PRIVATE
    GTest::GTest
    GTest::gmock
    Dtk${VERSION_SUFFIX}::Widget
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::CorePrivate
    Qt${QT_VERSION_MAJOR}::GuiPrivate
    ${THEME_LIBS}
    ${QT_LIBS}
    Qt${QT_VERSION_MAJOR}::WidgetsPrivate
    asan
    gcov
    PkgConfig::X11
)
target_compile_definitions(unit-tests PRIVATE
    DIALOG_SERVICE="org.deepin.fakefilemanager.filedialog"
    META_FILE_BASE_DIR="${CMAKE_SOURCE_DIR}"
    PLUGIN_OUTPUT_BASE_DIR="${PLUGIN_OUTPUT_BASE_DIR}"
)
target_include_directories(unit-tests PRIVATE
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine
    ${CMAKE_SOURCE_DIR}/iconengineplugins/dciiconengine
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon
    ${CMAKE_SOURCE_DIR}/platformthemeplugin
)
target_compile_definitions(unit-tests PRIVATE
    -DUT_PLATFORMPLUGIN_PATH="${CMAKE_BINARY_DIR}/plugins"
)
