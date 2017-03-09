#include "utility.h"
#include "qxcbintegration.h"
#include "qxcbconnection.h"
#include "qxcbscreen.h"

#include "dplatformintegration.h"
#include "dxcbwmsupport.h"

#include <QPixmap>
#include <QPainter>
#include <QCursor>
#include <QDebug>
#include <QtX11Extras/QX11Info>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <QtWidgets/qtwidgetsglobal.h>
#endif

#include <xcb/shape.h>

#include <X11/cursorfont.h>
#include <X11/Xlib.h>

#define _NET_WM_MOVERESIZE_MOVE              8   /* movement only */
#define _NET_WM_MOVERESIZE_CANCEL           11   /* cancel operation */

#define XATOM_MOVE_RESIZE "_NET_WM_MOVERESIZE"
#define XDEEPIN_BLUR_REGION "_NET_WM_DEEPIN_BLUR_REGION"
#define XDEEPIN_BLUR_REGION_ROUNDED "_NET_WM_DEEPIN_BLUR_REGION_ROUNDED"

QT_BEGIN_NAMESPACE
//extern Q_WIDGETS_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

DPP_BEGIN_NAMESPACE

QImage Utility::dropShadow(const QPixmap &px, qreal radius, const QColor &color)
{
    if (px.isNull())
        return QImage();

    QImage tmp(px.size() + QSize(radius * 2, radius * 2), QImage::Format_ARGB32_Premultiplied);
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPoint(radius, radius), px);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, radius, false, true);
    blurPainter.end();

    if (color == QColor(Qt::black))
        return blurred;

    tmp = blurred;

    // blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color);
    tmpPainter.end();

    return tmp;
}

QList<QRect> Utility::sudokuByRect(const QRect &rect, QMargins borders)
{
    QList<QRect> list;

//    qreal border_width = borders.left() + borders.right();

//    if ( border_width > rect.width()) {
//        borders.setLeft(borders.left() / border_width * rect.width());
//        borders.setRight(rect.width() - borders.left());
//    }

//    qreal border_height = borders.top() + borders.bottom();

//    if (border_height > rect.height()) {
//        borders.setTop(borders.top()/ border_height * rect.height());
//        borders.setBottom(rect.height() - borders.top());
//    }

    const QRect &contentsRect = rect - borders;

    list << QRect(0, 0, borders.left(), borders.top());
    list << QRect(list.at(0).topRight(), QSize(contentsRect.width(), borders.top())).translated(1, 0);
    list << QRect(list.at(1).topRight(), QSize(borders.right(), borders.top())).translated(1, 0);
    list << QRect(list.at(0).bottomLeft(), QSize(borders.left(), contentsRect.height())).translated(0, 1);
    list << contentsRect;
    list << QRect(contentsRect.topRight(), QSize(borders.right(), contentsRect.height())).translated(1, 0);
    list << QRect(list.at(3).bottomLeft(), QSize(borders.left(), borders.bottom())).translated(0, 1);
    list << QRect(contentsRect.bottomLeft(), QSize(contentsRect.width(), borders.bottom())).translated(0, 1);
    list << QRect(contentsRect.bottomRight(), QSize(borders.left(), borders.bottom())).translated(1, 1);

    return list;
}

QImage Utility::borderImage(const QPixmap &px, const QMargins &borders,
                            const QSize &size, QImage::Format format)
{
    QImage image(size, format);
    QPainter pa(&image);

    const QList<QRect> sudoku_src = sudokuByRect(px.rect(), borders);
    const QList<QRect> sudoku_tar = sudokuByRect(QRect(QPoint(0, 0), size), borders);

    pa.setCompositionMode(QPainter::CompositionMode_Source);

    for (int i = 0; i < 9; ++i) {
        pa.drawPixmap(sudoku_tar[i], px, sudoku_src[i]);
    }

    pa.end();

    return image;
}

xcb_atom_t Utility::internAtom(const char *name)
{
    if (!name || *name == 0)
        return XCB_NONE;

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(QX11Info::connection(), true, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(QX11Info::connection(), cookie, 0);

    if (!reply)
        return XCB_NONE;

    xcb_atom_t atom = reply->atom;
    free(reply);

    return atom;
}

void Utility::startWindowSystemMove(uint WId)
{
    sendMoveResizeMessage(WId, _NET_WM_MOVERESIZE_MOVE);
}

void Utility::cancelWindowMoveResize(uint WId)
{
    sendMoveResizeMessage(WId, _NET_WM_MOVERESIZE_CANCEL);
}

void Utility::setFrameExtents(uint WId, const QMargins &margins)
{
    xcb_atom_t frameExtents = internAtom("_GTK_FRAME_EXTENTS");

    if (frameExtents == XCB_NONE) {
        qWarning() << "Failed to create atom with name _GTK_FRAME_EXTENTS";
        return;
    }

    uint32_t value[4] = {
        (uint32_t)margins.left(),
        (uint32_t)margins.right(),
        (uint32_t)margins.top(),
        (uint32_t)margins.bottom()
    };

    xcb_change_property(QX11Info::connection(), XCB_PROP_MODE_REPLACE, WId, frameExtents, XCB_ATOM_CARDINAL, 32, 4, value);
}

void Utility::setInputShapeRectangles(uint WId, const QRegion &region)
{
    setInputShapeRectangles(WId, qregion2XcbRectangles(region));
}

void Utility::setInputShapeRectangles(uint WId, const QVector<xcb_rectangle_t> &rectangles)
{
    xcb_shape_rectangles(QX11Info::connection(), XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, XCB_CLIP_ORDERING_YX_BANDED, WId,
                         0, 0, rectangles.size(), rectangles.constData());
}

void Utility::setInputShapePath(uint WId, const QPainterPath &path)
{
    QVector<xcb_rectangle_t> rectangles;

    foreach(const QPolygonF &polygon, path.toFillPolygons()) {
        foreach(const QRect &area, QRegion(polygon.toPolygon()).rects()) {
            xcb_rectangle_t rectangle;

            rectangle.x = area.x();
            rectangle.y = area.y();
            rectangle.width = area.width();
            rectangle.height = area.height();

            rectangles.append(std::move(rectangle));
        }
    }

    setInputShapeRectangles(WId, rectangles);
}

void Utility::sendMoveResizeMessage(uint WId, uint32_t action, QPoint globalPos, Qt::MouseButton qbutton)
{
    int xbtn = qbutton == Qt::LeftButton ? XCB_BUTTON_INDEX_1 :
               qbutton == Qt::RightButton ? XCB_BUTTON_INDEX_3 :
               XCB_BUTTON_INDEX_ANY;

    if (globalPos.isNull())
        globalPos = QCursor::pos();

    xcb_client_message_event_t xev;

    xev.response_type = XCB_CLIENT_MESSAGE;
    xev.type = internAtom(XATOM_MOVE_RESIZE);
    xev.window = WId;
    xev.format = 32;
    xev.data.data32[0] = globalPos.x();
    xev.data.data32[1] = globalPos.y();
    xev.data.data32[2] = action;
    xev.data.data32[3] = xbtn;
    xev.data.data32[4] = 0;

    xcb_ungrab_pointer(QX11Info::connection(), QX11Info::appTime());
    xcb_send_event(QX11Info::connection(), false, QX11Info::appRootWindow(QX11Info::appScreen()),
                   XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                   (const char *)&xev);

    xcb_flush(QX11Info::connection());
}

QVector<xcb_rectangle_t> Utility::qregion2XcbRectangles(const QRegion &region)
{
    QVector<xcb_rectangle_t> rectangles;

    rectangles.reserve(region.rectCount());

    for (const QRect &rect : region.rects()) {
        xcb_rectangle_t r;

        r.x = rect.x();
        r.y = rect.y();
        r.width = rect.width();
        r.height = rect.height();

        rectangles << r;
    }

    return rectangles;
}

void Utility::startWindowSystemResize(uint WId, CornerEdge cornerEdge, const QPoint &globalPos)
{
    sendMoveResizeMessage(WId, cornerEdge, globalPos);
}

static xcb_cursor_t CornerEdge2Xcb_cursor_t(Utility::CornerEdge ce)
{
    switch (ce) {
    case Utility::TopEdge:
        return XC_top_side;
    case Utility::TopRightCorner:
        return XC_top_right_corner;
    case Utility::RightEdge:
        return XC_right_side;
    case Utility::BottomRightCorner:
        return XC_bottom_right_corner;
    case Utility::BottomEdge:
        return XC_bottom_side;
    case Utility::BottomLeftCorner:
        return XC_bottom_left_corner;
    case Utility::LeftEdge:
        return XC_left_side;
    case Utility::TopLeftCorner:
        return XC_top_left_corner;
    default:
        return XCB_CURSOR_NONE;
    }
}

bool Utility::setWindowCursor(uint WId, Utility::CornerEdge ce)
{
    const auto display = QX11Info::display();

    Cursor cursor = XCreateFontCursor(display, CornerEdge2Xcb_cursor_t(ce));

    if (!cursor) {
        qWarning() << "[ui]::setWindowCursor() call XCreateFontCursor() failed";
        return false;
    }

    const int result = XDefineCursor(display, WId, cursor);

    XFlush(display);

    return result == Success;
}

QRegion Utility::regionAddMargins(const QRegion &region, const QMargins &margins, const QPoint &offset)
{
    QRegion tmp;

    for (const QRect &rect : region.rects()) {
        tmp += rect.translated(offset) + margins;
    }

    return tmp;
}

QByteArray Utility::windowProperty(uint WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, quint32 len)
{
    QByteArray data;
    xcb_connection_t* conn = QX11Info::connection();
    xcb_get_property_cookie_t cookie = xcb_get_property(conn, false, WId, propAtom, typeAtom, 0, len);
    xcb_generic_error_t* err = nullptr;
    xcb_get_property_reply_t* reply = xcb_get_property_reply(conn, cookie, &err);

    if (reply != nullptr) {
        len = xcb_get_property_value_length(reply);
        const char* buf = static_cast<const char*>(xcb_get_property_value(reply));
        data.append(buf, len);
        free(reply);
    }

    if (err != nullptr) {
        free(err);
    }

    return data;
}

void Utility::setWindowProperty(uint WId, xcb_atom_t propAtom, xcb_atom_t typeAtom, const void *data, quint32 len, uint8_t format)
{
    xcb_connection_t* conn = QX11Info::connection();
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, WId, propAtom, typeAtom, format, len, data);
    xcb_flush(conn);
}

bool Utility::hasBlurWindow()
{
    return  DXcbWMSupport::instance()->hasBlurWindow();
}

bool Utility::blurWindowBackground(const uint WId, const QVector<BlurArea> &areas)
{
    if (!hasBlurWindow())
        return false;

    if (DXcbWMSupport::instance()->isDeepinWM()) {
        xcb_atom_t atom = DXcbWMSupport::instance()->_net_wm_deepin_blur_region_rounded_atom;

        if (atom == XCB_NONE)
            return false;

        if (areas.isEmpty()) {
            QVector<BlurArea> areas;

            areas << BlurArea();
        }

        setWindowProperty(WId, atom, XCB_ATOM_CARDINAL, areas.constData(), areas.size() * sizeof(BlurArea) / sizeof(quint32), sizeof(quint32) * 8);
    } else {
        xcb_atom_t atom = DXcbWMSupport::instance()->_kde_net_wm_blur_rehind_region_atom;

        if (atom == XCB_NONE)
            return false;

        QVector<quint32> rects;

        foreach (const BlurArea &area, areas) {
            if (area.xRadius <= 0 || area.yRaduis <= 0) {
                rects << area.x << area.y << area.width << area.height;
            } else {
                QPainterPath path;

                path.addRoundedRect(area.x, area.y, area.width, area.height, area.xRadius, area.yRaduis);

                foreach(const QPolygonF &polygon, path.toFillPolygons()) {
                    foreach(const QRect &area, QRegion(polygon.toPolygon()).rects()) {
                        rects << area.x() << area.y() << area.width() << area.height();
                    }
                }
            }
        }

        setWindowProperty(WId, atom, XCB_ATOM_CARDINAL, rects.constData(), rects.size(), sizeof(quint32) * 8);
    }

    return true;
}

bool Utility::blurWindowBackgroundByPaths(const uint WId, const QList<QPainterPath> &paths)
{
    if (DXcbWMSupport::instance()->isDeepinWM()) {
        QRect boundingRect;

        for (const QPainterPath &p : paths) {
            boundingRect |= p.boundingRect().toRect();
        }

        QImage image(boundingRect.size(), QImage::Format_Alpha8);

        image.fill(Qt::transparent);

        QPainter painter(&image);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(-boundingRect.topLeft());

        for (const QPainterPath &p : paths) {
            painter.fillPath(p, Qt::black);
        }

        return blurWindowBackgroundByImage(WId, boundingRect, image);
    } else {
        xcb_atom_t atom = DXcbWMSupport::instance()->_kde_net_wm_blur_rehind_region_atom;

        if (atom == XCB_NONE)
            return false;

        QVector<quint32> rects;

        foreach (const QPainterPath &path, paths) {
            foreach(const QPolygonF &polygon, path.toFillPolygons()) {
                foreach(const QRect &area, QRegion(polygon.toPolygon()).rects()) {
                    rects << area.x() << area.y() << area.width() << area.height();
                }
            }
        }

        setWindowProperty(WId, atom, XCB_ATOM_CARDINAL, rects.constData(), rects.size(), sizeof(quint32) * 8);
    }

    return true;
}

bool Utility::blurWindowBackgroundByImage(const uint WId, const QRect &blurRect, const QImage &maskImage)
{
    if (!DXcbWMSupport::instance()->isDeepinWM() || maskImage.format() != QImage::Format_Alpha8)
        return false;

    QByteArray array;
    QVector<qint32> area;

    area.reserve(5);
    area << blurRect.x() << blurRect.y() << blurRect.width() << blurRect.height() << maskImage.bytesPerLine();

    array.reserve(area.size() * sizeof(qint32) / sizeof(char) * area.size() + maskImage.byteCount());
    array.append((const char*)area.constData(), sizeof(qint32) / sizeof(char) * area.size());
    array.append((const char*)maskImage.constBits(), maskImage.byteCount());

    setWindowProperty(WId, DXcbWMSupport::instance()->_net_wm_deepin_blur_region_mask,
                      DXcbWMSupport::instance()->_net_wm_deepin_blur_region_mask,
                      array.constData(), array.length(), 8);

    return true;
}

DPP_END_NAMESPACE

QT_BEGIN_NAMESPACE
QDebug operator<<(QDebug deg, const Utility::BlurArea &area)
{
    QDebugStateSaver saver(deg);
    Q_UNUSED(saver)

    deg.setAutoInsertSpaces(true);
    deg << "x:" << area.x
        << "y:" << area.y
        << "width:" << area.width
        << "height:" << area.height
        << "xRadius:" << area.xRadius
        << "yRadius:" << area.yRaduis;

    return deg;
}
QT_END_NAMESPACE
