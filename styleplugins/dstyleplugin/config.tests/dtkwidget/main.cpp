/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <dplatformwindowhandle.h>
#include <QWindow>

int main()
{
    QWindow *w = new QWindow();

    Dtk::Widget::DPlatformWindowHandle handle((QWidget *)w);

    handle.setEnableBlurWindow(true);

    return 0;
}
