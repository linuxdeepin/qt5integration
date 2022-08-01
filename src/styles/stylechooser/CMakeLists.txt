PROJECT(stylechooser)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
list(APPEND COMMON_LIBS
    Qt5::Widgets
)

set(PLUGIN_TYPE styles)
set(PLUGIN_NAME chameleon)
include(../../chooser.cmake)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${Qt5Widgets_INCLUDE_DIRS}
)
