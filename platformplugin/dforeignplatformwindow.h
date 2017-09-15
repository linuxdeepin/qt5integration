/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
