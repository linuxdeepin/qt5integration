# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(builtinengine)
add_subdirectory(dciiconengine)
add_subdirectory(diconproxyengine)
add_subdirectory(svgiconengine)
if(ENABLE_QT_XDG_ICON_LOADER)
  add_subdirectory(xdgiconproxyengine)
endif()
