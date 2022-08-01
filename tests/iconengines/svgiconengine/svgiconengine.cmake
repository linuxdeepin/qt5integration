if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif()

include_directories(${CMAKE_SOURCE_DIR}/iconengineplugins/svgiconengine)

list(APPEND GLOBAL_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ut_qsvgiconengine.cpp
)
list(APPEND RCC_QRCS
    ${CMAKE_CURRENT_LIST_DIR}/icons.qrc
)

