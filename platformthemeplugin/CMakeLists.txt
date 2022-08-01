PROJECT(qdeepin)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

set(REQUIRED_QT_VERSION 5.15.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets DBus X11Extras)
find_package(DtkCore REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(QT5XDG REQUIRED)
find_package(qt5xdgiconloader REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS ThemeSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_COMPILER_PREDEFINES ON)

add_definitions(-DXDG_ICON_VERSION_MAR=${qt5xdgiconloader_VERSION_MAJOR})
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

set(GLOBAL_HEADERS
    qdeepinfiledialoghelper.h
    diconproxyengine.h
    dthemesettings.h
    xdgiconenginecreator.h
)
set(GLOBAL_SOURCES
    qdeepintheme.cpp
    qdeepinfiledialoghelper.cpp
    diconproxyengine.cpp
    dthemesettings.cpp
    xdgiconenginecreator.cpp
)
set(RCC_QRCS
    icons/deepin-theme-plugin-icons.qrc
    deepin-theme-plugin.qrc
)
try_compile(DTHEMED_ICON_COMPILE_RESULT ${CMAKE_CURRENT_BINARY_DIR}/dthemed_icon_lookup
    ${CMAKE_CURRENT_LIST_DIR}/config.tests/dthemed_icon_lookup dthemed_icon_lookup
)

if(DTHEMED_ICON_COMPILE_RESULT)
    add_definitions(-DDTHEMED_ICON_LOOKUP)
    list(APPEND GLOBAL_HEADERS diconengine.h)
    list(APPEND GLOBAL_SOURCES diconengine.cpp)
endif()
set(DBUS_INTERFACE_FILEDIALOGMANAGER xmls/com.deepin.filemanager.filedialogmanager.xml)
qt5_add_dbus_interface(DBUS_INTERFACE_XMLS ${DBUS_INTERFACE_FILEDIALOGMANAGER} filedialogmanager_interface)

set(DBUS_INTERFACE_FILEDIALOG xmls/com.deepin.filemanager.filedialog.xml)
qt5_add_dbus_interface(DBUS_INTERFACE_XMLS ${DBUS_INTERFACE_FILEDIALOG} filedialog_interface)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/plugins/platformthemes/${BASED_DTK_DIR})
add_library(${PROJECT_NAME} MODULE main.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS} ${RCC_QRCS} ${DBUS_INTERFACE_XMLS})

set(COMMON_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::X11Extras
    ${DtkWidget_LIBRARIES}
    Qt5Xdg
    Qt5::ThemeSupport
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${COMMON_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
    Qt5::X11Extras
    ${Qt5ThemeSupport_PRIVATE_INCLUDE_DIRS}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/platformthemes/${BASED_DTK_DIR})
