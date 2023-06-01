# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.10)

set(DTK_VERSION
    6.0.0
    CACHE STRING "Project version")

project(
  qtintegration
  VERSION ${DTK_VERSION}
  DESCRIPTION "Qt platform integration for DTK"
  HOMEPAGE_URL "https://github.com/linuxdeepin/qt5integration"
  LANGUAGES CXX C)

option(BUILD_TESTS "Build tests" OFF)

if(PROJECT_VERSION_MAJOR EQUAL 6)
  set(VERSION_SUFFIX 6)
  set(CMAKE_CXX_STANDARD 17)
  find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
else()
  set(CMAKE_CXX_STANDARD 14)
  find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX /usr)
endif()
include(GNUInstallDirs)
include(CMakeParseArguments)
include(CMakeDependentOption)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Release)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
endif()

# Some util functions
function(add_plugin)
  set(options EXECUTABLE)
  set(oneValueArgs NAME OUTPUT_NAME OUTPUT_DIR INSTALL_DIR)
  set(multiValueArgs SOURCES HEADERS RESOURCES DEPENDENCIES DEFINITIONS OPTIONS INCLUDE_DIRS)
  cmake_parse_arguments(TARGET "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})
  if(TARGET_EXECUTABLE)
    add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESOURCES})
  else()
    add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESOURCES})
  endif()
  target_compile_definitions(${TARGET_NAME} PRIVATE ${TARGET_DEFINITIONS})
  target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_OPTIONS})
  target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_DEPENDENCIES})
  target_include_directories(${TARGET_NAME} PRIVATE ${TARGET_INCLUDE_DIRS})
  if(TARGET_OUTPUT_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
  endif()
  if(TARGET_OUTPUT_DIR)
    set_target_properties(${TARGET_NAME}
                          PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR})
  endif()
  install(TARGETS ${TARGET_NAME} DESTINATION ${TARGET_INSTALL_DIR})
endfunction()

find_package(Dtk${VERSION_SUFFIX}Gui REQUIRED)
find_package(Qt5XdgIconLoader)
find_package(Qt5Xdg)
cmake_dependent_option(ENABLE_QT_XDG_ICON_LOADER "Enable QtXdgIconLoader" ON "QT_VERSION_MAJOR EQUAL 5;Qt5XdgIconLoader_FOUND;Qt5Xdg_FOUND" OFF)
if(ENABLE_QT_XDG_ICON_LOADER)
  set(QT_XDG_ICON_DEPS Qt5XdgIconLoader Qt5Xdg)
  set(XDG_ICON_VERSION_MAJOR ${Qt5XdgIconLoader_VERSION_MAJOR})
endif()

set(PLUGIN_OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/plugins CACHE STRING "Plugin output base path")
set(PLUGIN_INSTALL_BASE_DIR ${CMAKE_INSTALL_LIBDIR}/qt${QT_VERSION_MAJOR}/plugins CACHE STRING "Plugin install base path")

add_subdirectory(iconengineplugins)
add_subdirectory(imageformatplugins)
add_subdirectory(platformthemeplugin)
add_subdirectory(styleplugins)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_subdirectory(styles)
endif()
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

set(VARIABLES
    CACHE STRING "Variables to print")

foreach(VARIABLE ${VARIABLES})
  message(STATUS "${VARIABLE}: ${${VARIABLE}}")
endforeach()
