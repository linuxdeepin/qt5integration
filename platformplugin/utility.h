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

#ifndef UTILITY_H
#define UTILITY_H

#include <QImage>

#include "global.h"

QT_BEGIN_NAMESPACE
class QXcbWindow;
QT_END_NAMESPACE

typedef uint32_t xcb_atom_t;

DPP_BEGIN_NAMESPACE

class Utility
{
public:
    enum CornerEdge {
        TopLeftCorner = 0,
        TopEdge = 1,
        TopRightCorner = 2,
        RightEdge = 3,
        BottomRightCorner = 4,
        BottomEdge = 5,
        BottomLeftCorner = 6,
        LeftEdge = 7
    };

    static QImage dropShadow(const QPixmap &px, qreal radius, const QColor &color);
    static QImage borderImage(const QPixmap &px, const QMargins &borders, const QSize &size,
                              QImage::Format format = QImage::Format_ARGB32_Premultiplied);

    static QList<QRect> sudokuByRect(const QRect &rect, QMargins borders);

    static xcb_atom_t internAtom(const char *name);
    static void startWindowSystemMove(quint32 WId);
    static void cancelWindowMoveResize(quint32 WId);
    static void setFrameExtents(quint32 WId, const QMargins &margins);
    static void setShapeRectangles(quint32 WId, const QRegion &region, bool onlyInput = true);
    static void setShapePath(quint32 WId, const QPainterPath &path, bool onlyInput = true);
    static void startWindowSystemResize(quint32 WId, CornerEdge cornerEdge, const QPoint &globalPos = QPoint());
    static bool setWindowCursor(quint32 WId, CornerEdge ce);

    static QRegion regionAddMargins(const QRegion &region, const QMargins &margins, const QPoint &offset = QPoint(0, 0));

    static QByteArray windowProperty(quint32 WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, quint32 len);
    static void setWindowProperty(quint32 WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, const void *data, quint32 len, uint8_t format = 8);

    struct BlurArea {
        qint32 x;
        qint32 y;
        qint32 width;
        qint32 height;
        qint32 xRadius;
        qint32 yRaduis;

        inline BlurArea operator *(qreal scale)
        {
            BlurArea new_area;

            new_area.x = x * scale;
            new_area.y = y * scale;
            new_area.width = width * scale;
            new_area.height = height * scale;
            new_area.xRadius = xRadius;
            new_area.yRaduis = yRaduis;

            return new_area;
        }

        inline BlurArea &operator *=(qreal scale)
        {
            x *= scale;
            y *= scale;
            width *= scale;
            height *= scale;

            return *this;
        }
    };

    // by Deepin Window Manager
    static bool hasBlurWindow();
    static bool hasComposite();
    static bool blurWindowBackground(const quint32 WId, const QVector<BlurArea> &areas);
    static bool blurWindowBackgroundByPaths(const quint32 WId, const QList<QPainterPath> &paths);
    static bool blurWindowBackgroundByImage(const quint32 WId, const QRect &blurRect, const QImage &maskImage);

    static quint32 getWorkspaceForWindow(quint32 WId);
    static QVector<quint32> getWindows();
    static QVector<quint32> getCurrentWorkspaceWindows();

    struct QtMotifWmHints {
        quint32 flags, functions, decorations;
        qint32 input_mode;
        quint32 status;
    };

    static QtMotifWmHints getMotifWmHints(quint32 WId);
    static void setMotifWmHints(quint32 WId, const QtMotifWmHints &hints);
    static quint32 getNativeTopLevelWindow(quint32 WId);

    static QPoint translateCoordinates(const QPoint &pos, quint32 src, quint32 dst);

#ifdef Q_OS_LINUX
    static int XIconifyWindow(void *display, quint32 w, int screen_number);
#endif

private:
    static void sendMoveResizeMessage(quint32 WId, uint32_t action, QPoint globalPos = QPoint(), Qt::MouseButton qbutton = Qt::LeftButton);
    static QWindow *getWindowById(quint32 WId);
    static qreal getWindowDevicePixelRatio(quint32 WId);
};

DPP_END_NAMESPACE

QT_BEGIN_NAMESPACE
DPP_USE_NAMESPACE
QDebug operator<<(QDebug deg, const Utility::BlurArea &area);
inline QPainterPath operator *(const QPainterPath &path, qreal scale)
{
    if (qFuzzyCompare(1.0, scale))
        return path;

    QPainterPath new_path = path;

    for (int i = 0; i < path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);

        new_path.setElementPositionAt(i, e.x * scale, e.y * scale);
    }

    return new_path;
}
inline QPainterPath &operator *=(QPainterPath &path, qreal scale)
{
    for (int i = 0; i < path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);

        path.setElementPositionAt(i, e.x * scale, e.y * scale);
    }

    return path;
}
QT_END_NAMESPACE

#endif // UTILITY_H
