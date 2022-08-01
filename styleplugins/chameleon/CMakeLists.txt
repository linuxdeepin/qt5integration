PROJECT(chameleon)
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
    common.h
    chameleonstyle.h
    ../dstyleplugin/dstyleanimation.h
)
set(GLOBAL_SOURCES
    chameleonstyle.cpp
    ../dstyleplugin/dstyleanimation.cpp
)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/plugins/styles/${BASED_DTK_DIR})
add_library(${PROJECT_NAME} SHARED main.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS} resources.qrc)

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
