CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

if (NOT BASED_DTK_DIR)
    set(BASED_DTK_DIR "based-dtk" CACHE STRING "based dtk directory")
endif()

project(qt5integration)

add_subdirectory(src)
add_subdirectory(styleplugins)
add_subdirectory(platformthemeplugin)
add_subdirectory(iconengineplugins)
add_subdirectory(imageformatplugins)
add_subdirectory(tests)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(styles)
endif()
