# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

if(QT_VERSION_MAJOR EQUAL 5)
    qt5_add_dbus_interface(DBUS_INTERFACES ${CMAKE_CURRENT_LIST_DIR}/xmls/com.deepin.filemanager.filedialog.xml filedialog_interface)
    qt5_add_dbus_interface(DBUS_INTERFACES ${CMAKE_CURRENT_LIST_DIR}/xmls/com.deepin.filemanager.filedialogmanager.xml filedialogmanager_interface)
else()
    qt6_add_dbus_interface(DBUS_INTERFACES ${CMAKE_CURRENT_LIST_DIR}/xmls/com.deepin.filemanager.filedialog.xml filedialog_interface)
    qt6_add_dbus_interface(DBUS_INTERFACES ${CMAKE_CURRENT_LIST_DIR}/xmls/com.deepin.filemanager.filedialogmanager.xml filedialogmanager_interface)
endif()

set(DFM_DLG_HEADERS ${CMAKE_CURRENT_LIST_DIR}/qdeepinfiledialoghelper.h)
set(DFM_DLG_SOURCES ${CMAKE_CURRENT_LIST_DIR}/qdeepinfiledialoghelper.cpp)
