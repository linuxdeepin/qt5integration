# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
add_subdirectory(chameleon)
if(ENABLE_OLD_STYLE)
    add_subdirectory(dstyleplugin)
endif()
