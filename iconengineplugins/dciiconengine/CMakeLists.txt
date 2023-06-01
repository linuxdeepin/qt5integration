# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

add_plugin(
    NAME
      ddciicon
    OUTPUT_DIR
      ${PLUGIN_OUTPUT_BASE_DIR}/iconengines
    INSTALL_DIR
      ${PLUGIN_INSTALL_BASE_DIR}/iconengines
    SOURCES
      dciiconengine.cpp
      main.cpp
    HEADERS
      dciiconengine.h
    DEPENDENCIES
      Qt${QT_VERSION_MAJOR}::GuiPrivate
      Dtk${VERSION_SUFFIX}::Gui
)
