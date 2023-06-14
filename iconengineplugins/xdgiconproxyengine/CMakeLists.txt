# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

dtk_add_plugin(
    NAME
      xdgiconproxy
    OUTPUT_DIR
      ${PLUGIN_OUTPUT_BASE_DIR}/iconengines
    INSTALL_DIR
      ${PLUGIN_INSTALL_BASE_DIR}/iconengines
    SOURCES
      xdgiconproxyengine.cpp
      main.cpp
    HEADERS
      xdgiconproxyengine.h
    DEPENDENCIES
      Qt${QT_VERSION_MAJOR}::GuiPrivate
      Dtk${VERSION_SUFFIX}::Gui
      ${QT_XDG_ICON_DEPS}
    DEFINITIONS
      -DXDG_ICON_VERSION_MAR=${XDG_ICON_VERSION_MAJOR}
)
