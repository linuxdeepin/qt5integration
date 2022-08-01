PROJECT(dstyleplugin)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(DtkCore REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkWidget REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_COMPILER_PREDEFINES ON)

set(GLOBAL_HEADERS
    style.h
    common.h
    colorutils.h
    geometryutils.h
    paletteextended.h
    commonhelper.h
    painterhelper.h
    dstyleanimation.h
    hidpihelper.h
)
set(GLOBAL_SOURCES
    style.cpp
    sliderhelper.cpp
    colorutils.cpp
    geometryutils.cpp
    paletteextended.cpp
    pushbuttonhelper.cpp
    framehelper.cpp
    lineedithelper.cpp
    commonhelper.cpp
    scrollbarhelper.cpp
    painterhelper.cpp
    menuhelper.cpp
    dstyleanimation.cpp
    progressbarhelper.cpp
    checkboxhelper.cpp
    rubberbandhelper.cpp
    headerviewhelper.cpp
    comboboxhelper.cpp
    toolbuttonhelper.cpp
    tabwidgethelper.cpp
    spinboxhelper.cpp
    hidpihelper.cpp
    tabbarhelper.cpp
)
set(RCC_QRCS
    brushchemes.qrc
    assets_light.qrc
    assets_dark.qrc
    assets_semidark.qrc
    assets_semilight.qrc
    assets.qrc
)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/plugins/styles/${BASED_DTK_DIR})
add_library(${PROJECT_NAME} MODULE dstyleplugin.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS} ${RCC_QRCS})

set(COMMON_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${DtkWidget_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${COMMON_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/styles/${BASED_DTK_DIR})
