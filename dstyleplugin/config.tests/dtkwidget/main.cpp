/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include <dplatformwindowhandle.h>
#include <QWindow>

int main()
{
    QWindow *w = new QWindow();

    DPlatformWindowHandle handle(w);

    handle.setEnableBlurWindow(true);

    return 0;
}
