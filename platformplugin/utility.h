#ifndef UTILITY_H
#define UTILITY_H

#include <QImage>

#include <xcb/xproto.h>

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
    static void startWindowSystemMove(uint WId);
    static void cancelWindowMoveResize(uint WId);
    static void setFrameExtents(uint WId, const QMargins &margins);
    static void setInputShapeRectangles(uint WId, const QRegion &region);
    static void setInputShapeRectangles(uint WId, const QVector<xcb_rectangle_t> &rectangles);
    static void setInputShapePath(uint WId, const QPainterPath &path);
    static void startWindowSystemResize(uint WId, CornerEdge cornerEdge, const QPoint &globalPos = QPoint());
    static bool setWindowCursor(uint WId, CornerEdge ce);

    static QRegion regionAddMargins(const QRegion &region, const QMargins &margins, const QPoint &offset = QPoint(0, 0));

    static QByteArray windowProperty(uint WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, quint32 len);
    static void setWindowProperty(uint WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, const void *data, quint32 len, uint8_t format = 8);

    struct BlurArea {
        qint32 x;
        qint32 y;
        qint32 width;
        qint32 height;
        qint32 xRadius;
        qint32 yRaduis;
    };

    // by Deepin Window Manager
    static bool hasBlurWindow();
    static bool blurWindowBackground(const uint WId, const QVector<BlurArea> &areas);
    static bool blurWindowBackgroundByPaths(const uint WId, const QList<QPainterPath> &paths);
    static bool blurWindowBackgroundByImage(const uint WId, const QRect &blurRect, const QImage &maskImage);

private:
    static void sendMoveResizeMessage(uint WId, uint32_t action, QPoint globalPos = QPoint(), Qt::MouseButton qbutton = Qt::LeftButton);
    static QVector<xcb_rectangle_t> qregion2XcbRectangles(const QRegion &region);
};

DPP_END_NAMESPACE

QT_BEGIN_NAMESPACE
DPP_USE_NAMESPACE
QDebug operator<<(QDebug deg, const Utility::BlurArea &area);
QT_END_NAMESPACE

#endif // UTILITY_H