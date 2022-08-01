include_directories(${CMAKE_SOURCE_DIR}/iconengineplugins/builtinengine)

list(APPEND GLOBAL_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ut_builtinengine.cpp
)
list(APPEND RCC_QRCS
    ${CMAKE_CURRENT_LIST_DIR}/icons/theme-icons.qrc
)

