/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef DFOREIGNPLATFORMWINDOW_H
#define DFOREIGNPLATFORMWINDOW_H

#include "global.h"

#include <QtGlobal>

#ifdef Q_OS_LINUX
#define private public
#include "qxcbwindow.h"
typedef QXcbWindow QNativeWindow;
#undef private
#elif defined(Q_OS_WIN)
#include "qwindowswindow.h"
typedef QWindowsWindow QNativeWindow;
#endif

DPP_BEGIN_NAMESPACE

class DForeignPlatformWindow : public QNativeWindow
{
public:
    explicit DForeignPlatformWindow(QWindow *window);
    ~DForeignPlatformWindow();

    QRect geometry() const Q_DECL_OVERRIDE;

private:
    void updateTitle();
    void updateWmClass();
    void updateWmDesktop();
    void updateWindowState();
    void updateWindowTypes();
    void updateProcessId();

    void init();
};

DPP_END_NAMESPACE

#endif // DFOREIGNPLATFORMWINDOW_H
