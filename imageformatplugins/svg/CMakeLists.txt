PROJECT(dsvg)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

find_package(Qt5 REQUIRED COMPONENTS Core Gui)
find_package(DtkCore REQUIRED)
find_package(DtkGui REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_COMPILER_PREDEFINES ON)

set(GLOBAL_HEADERS
    qsvgiohandler.h
)
set(GLOBAL_SOURCES
    qsvgiohandler.cpp
)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/plugins/imageformats/${BASED_DTK_DIR})
add_library(${PROJECT_NAME} MODULE main.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS})

set(COMMON_LIBS
    Qt5::Core
    Qt5::Gui
    ${DtkGui_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${COMMON_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${DtkGui_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/imageformats/${BASED_DTK_DIR})
