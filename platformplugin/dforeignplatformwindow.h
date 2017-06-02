/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
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

    void init();
};

DPP_END_NAMESPACE

#endif // DFOREIGNPLATFORMWINDOW_H
