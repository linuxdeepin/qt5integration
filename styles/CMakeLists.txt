# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
add_executable(styles
    norwegianwoodstyle.h
    widgetgallery.h
    main.cpp
    norwegianwoodstyle.cpp
    widgetgallery.cpp
    styles.qrc
)

target_link_libraries(styles PRIVATE Dtk${VERSION_SUFFIX}::Widget)
