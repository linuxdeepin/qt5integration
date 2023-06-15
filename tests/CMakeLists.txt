# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

add_executable(unit-tests
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine/dbuiltiniconengine.h
    ${CMAKE_SOURCE_DIR}/iconengineplugins/dciiconengine/dciiconengine.h
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.h
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.h
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci/qdciiohandler.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/common.h
    ${CMAKE_SOURCE_DIR}/styleplugins/dstyleplugin/dstyleanimation.h
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine/dbuiltiniconengine.cpp
    ${CMAKE_SOURCE_DIR}/iconengineplugins/dciiconengine/dciiconengine.cpp
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.cpp
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.cpp
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci/qdciiohandler.cpp
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.cpp
    ${CMAKE_SOURCE_DIR}/styleplugins/dstyleplugin/dstyleanimation.cpp
    iconengines/builtinengine/ut_builtinengine.cpp
    iconengines/builtinengine/icons/theme-icons.qrc
    iconengines/dciiconengine/ut_dciiconengine.cpp
    iconengines/dciiconengine/icons/dtk_dci_test_res.qrc
    iconengines/svgiconengine/ut_qsvgiconengine.cpp
    iconengines/svgiconengine/icons.qrc
    imageformats/dci/ut_qdciiohandler.cpp
    imageformats/dci/dci_test_files.qrc
    imageformats/svg/ut_qsvgiohandler.cpp
    styleplugins/chameleon/ut_chameleonstyle.cpp
    main.cpp
)
find_package(GTest REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Concurrent REQUIRED)
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
    Qt${QT_VERSION_MAJOR}::WidgetsPrivate
    asan
    gcov
)
target_include_directories(unit-tests PRIVATE
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine
    ${CMAKE_SOURCE_DIR}/iconengineplugins/dciiconengine
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg
    ${CMAKE_SOURCE_DIR}/imageformatplugins/dci
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon
)
target_compile_definitions(unit-tests PRIVATE
    -DUT_PLATFORMPLUGIN_PATH="${CMAKE_BINARY_DIR}/plugins"
)
