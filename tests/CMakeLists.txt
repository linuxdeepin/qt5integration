PROJECT(tests)
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(DtkCore REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(AUTOMOC_COMPILER_PREDEFINES ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-access-control")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-access-control")
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include(iconengines/iconengines.cmake)
include(styleplugins/styleplugins.cmake)
include(imageformats/imageformats.cmake)

add_definitions(-DUT_PLATFORMPLUGIN_PATH=\"${CMAKE_SOURCE_DIR}/bin/plugins/platformthemes\")

list(APPEND GLOBAL_HEADERS
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine/dbuiltiniconengine.h
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.h
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.h
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/common.h
)
list(APPEND GLOBAL_SOURCES
    ${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine/dbuiltiniconengine.cpp
    ${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine/qsvgiconengine.cpp
    ${CMAKE_SOURCE_DIR}/imageformatplugins/svg/qsvgiohandler.cpp
    ${CMAKE_SOURCE_DIR}/styleplugins/chameleon/chameleonstyle.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${GLOBAL_SOURCES} ${GLOBAL_HEADERS} ${RCC_QRCS})

set(COMMON_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Concurrent
    ${DtkWidget_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${COMMON_LIBS} chameleon)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/bin/plugins/styles/${BASED_DTK_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC
    Qt5::Concurrent
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
)
