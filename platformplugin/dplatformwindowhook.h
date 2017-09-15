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

#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QtGlobal>

#ifdef Q_OS_LINUX
#define private public
#include "qxcbwindow.h"
#include "qxcbclipboard.h"
typedef QXcbWindow QNativeWindow;
#undef private
#elif defined(Q_OS_WIN)
#include "qwindowswindow.h"
typedef QWindowsWindow QNativeWindow;
#endif

#include "global.h"

DPP_BEGIN_NAMESPACE

class DPlatformWindowHook : public QObject
{
    Q_OBJECT

public:
    DPlatformWindowHook(QNativeWindow *window);

    ~DPlatformWindowHook();

    QNativeWindow *window() const
    { return static_cast<QNativeWindow*>(reinterpret_cast<QPlatformWindow*>(const_cast<DPlatformWindowHook*>(this)));}

    DPlatformWindowHook *me() const;

    void setGeometry(const QRect &rect);
    QRect geometry() const;

    QMargins frameMargins() const;

    void setParent(const QPlatformWindow *window);

    void setWindowTitle(const QString &title);
    void setWindowIcon(const QIcon &icon);

    QPoint mapToGlobal(const QPoint &pos) const;
    QPoint mapFromGlobal(const QPoint &pos) const;

    void setMask(const QRegion &region);
#ifdef Q_OS_LINUX
    void setWindowState(Qt::WindowState state);
    void setVisible(bool visible);
    void requestActivateWindow();
#endif
//    bool startSystemResize(const QPoint &pos, Qt::Corner corner);

    void propagateSizeHints();

    static DPlatformWindowHook *getHookByWindow(const QPlatformWindow *window);

signals:
    void windowGeometryAboutChanged(const QRect &rect);

private:
    void setWindowMargins(const QMargins &margins, bool propagateSizeHints = false);

    QMargins windowMargins;
    static QHash<const QPlatformWindow*, DPlatformWindowHook*> mapped;

    QNativeWindow *nativeWindow;

    friend class DPlatformBackingStore;
};

DPP_END_NAMESPACE

Q_DECLARE_METATYPE(QPainterPath)

#endif // TESTWINDOW_H
