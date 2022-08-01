PROJECT(styles)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)


find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(DtkCore REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkWidget REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_COMPILER_PREDEFINES ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-access-control")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-access-control")
set(GLOBAL_HEADERS
    norwegianwoodstyle.h
    widgetgallery.h
)
set(GLOBAL_SOURCES
    norwegianwoodstyle.cpp
    widgetgallery.cpp
)
set(RCC_QRCS
    styles.qrc
)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(${PROJECT_NAME} main.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS} ${RCC_QRCS})

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
