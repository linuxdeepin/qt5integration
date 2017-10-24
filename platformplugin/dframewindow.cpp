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

#include "dframewindow.h"

#ifdef Q_OS_LINUX
#include "dwmsupport.h"
#include "qxcbwindow.h"
#endif

#include <QPainter>
#include <QPaintEvent>
#include <QGuiApplication>
#include <QDebug>

#include <private/qguiapplication_p.h>

DPP_BEGIN_NAMESPACE

QList<DFrameWindow*> DFrameWindow::frameWindowList;

DFrameWindow::DFrameWindow()
    : QRasterWindow()
{
    QSurfaceFormat f = this->format();
    f.setAlphaBufferSize(8);
    setFormat(f);

    m_cursorAnimation.setDuration(50);
    m_cursorAnimation.setEasingCurve(QEasingCurve::InExpo);

    connect(&m_cursorAnimation, &QVariantAnimation::valueChanged,
            this, [this] (const QVariant &value) {
        QCursor::setPos(value.toPoint());
    });

    m_startAnimationTimer.setSingleShot(true);
    m_startAnimationTimer.setInterval(300);

    connect(&m_startAnimationTimer, &QTimer::timeout,
            this, &DFrameWindow::startCursorAnimation);

    updateContentMarginsHint();

    QGuiApplicationPrivate::window_list.removeAll(this);
    frameWindowList.append(this);
}

DFrameWindow::~DFrameWindow()
{
    frameWindowList.removeOne(this);
}

int DFrameWindow::shadowRadius() const
{
    return m_shadowRadius;
}

void DFrameWindow::setShadowRaduis(int radius)
{
    if (m_shadowRadius == radius)
        return;

    m_shadowRadius = radius;

    updateContentMarginsHint();
}

QPoint DFrameWindow::shadowOffset() const
{
    return m_shadowOffset;
}

void DFrameWindow::setShadowOffset(const QPoint &offset)
{
    if (m_shadowOffset == offset)
        return;

    m_shadowOffset = offset;

    updateContentMarginsHint();
}

QColor DFrameWindow::shadowColor() const
{
    return m_shadowColor;
}

void DFrameWindow::setShadowColor(const QColor &color)
{
    if (m_shadowColor == color)
        return;

    m_shadowColor = color;

    updateShadowPixmap();
    update();
}

int DFrameWindow::borderWidth() const
{
    return m_borderWidth;
}

void DFrameWindow::setBorderWidth(int width)
{
    if (m_borderWidth == width)
        return;

    m_borderWidth = width;

    updateContentMarginsHint();
}

QColor DFrameWindow::borderColor() const
{
    return m_borderColor;
}

void DFrameWindow::setBorderColor(const QColor &color)
{
    if (m_borderColor == color)
        return;

    m_borderColor = color;

    updateShadowPixmap();
    update();
}

QPainterPath DFrameWindow::contentPath() const
{
    return m_clipPathOfContent;
}

inline static QSize margins2Size(const QMargins &margins)
{
    return QSize(margins.left() + margins.right(),
                 margins.top() + margins.bottom());
}

void DFrameWindow::setContentPath(const QPainterPath &path)
{
    setContentPath(path, false);
}

void DFrameWindow::setContentRoundedRect(const QRect &rect, int radius)
{
    QPainterPath path;

    path.addRoundedRect(rect, radius, radius);
    m_contentGeometry = rect.translated(contentOffsetHint());
    setContentPath(path, true, radius);
}

QMargins DFrameWindow::contentMarginsHint() const
{
    return m_contentMarginsHint;
}

QPoint DFrameWindow::contentOffsetHint() const
{
    return QPoint(m_contentMarginsHint.left(), m_contentMarginsHint.top());
}

bool DFrameWindow::isClearContentAreaForShadowPixmap() const
{
    return m_clearContent;
}

void DFrameWindow::setClearContentAreaForShadowPixmap(bool clear)
{
    if (m_clearContent == clear)
        return;

    m_clearContent = clear;

    if (clear && !m_shadowPixmap.isNull()) {
        QPainter pa(&m_shadowPixmap);

        pa.setCompositionMode(QPainter::CompositionMode_Clear);
        pa.setRenderHint(QPainter::Antialiasing);
        pa.fillPath(m_clipPathOfContent.translated(QPoint(m_shadowRadius, m_shadowRadius) - m_shadowOffset) * devicePixelRatio(), Qt::transparent);
        pa.end();
    }
}

bool DFrameWindow::isEnableSystemResize() const
{
    return m_enableSystemResize;
}

void DFrameWindow::setEnableSystemResize(bool enable)
{
    m_enableSystemResize = enable;

    if (!m_enableSystemResize)
        Utility::cancelWindowMoveResize(Utility::getNativeTopLevelWindow(winId()));
}

bool DFrameWindow::isEnableSystemMove() const
{
#ifdef Q_OS_LINUX
    if (!m_enableSystemMove)
        return false;

    quint32 hints = DXcbWMSupport::getMWMFunctions(Utility::getNativeTopLevelWindow(winId()));

    return (hints == DXcbWMSupport::MWM_FUNC_ALL || hints & DXcbWMSupport::MWM_FUNC_MOVE);
#endif

    return m_enableSystemMove;
}

void DFrameWindow::setEnableSystemMove(bool enable)
{
    m_enableSystemMove = enable;

    if (!m_enableSystemMove) {
        qApp->setOverrideCursor(cursor());

        cancelAdsorbCursor();
        m_canAdsorbCursor = false;

        Utility::cancelWindowMoveResize(Utility::getNativeTopLevelWindow(winId()));
    }
}

void DFrameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter pa(this);
    QPoint offset = m_contentGeometry.topLeft() - contentOffsetHint();

    pa.drawPixmap(offset, m_shadowPixmap);
}

void DFrameWindow::showEvent(QShowEvent *event)
{
    // Set frame extents
    Utility::setFrameExtents(winId(), contentMarginsHint() * devicePixelRatio());

    updateShadowPixmap();

    return QRasterWindow::showEvent(event);
}

void DFrameWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->source() == Qt::MouseEventSynthesizedByQt && qApp->mouseButtons() == Qt::LeftButton
            && m_clipPathOfContent.contains(event->pos() - m_contentGeometry.topLeft())) {
        if (!isEnableSystemMove())
            return;

        ///TODO: Warning: System move finished no mouse release event
        Utility::startWindowSystemMove(Utility::getNativeTopLevelWindow(winId()));

        return;
    }

    if (!canResize())
        return;

    qApp->setOverrideCursor(cursor());

    if (qApp->mouseButtons() != Qt::LeftButton && m_contentGeometry.contains(event->pos())) {
        return;
    }

    bool isFixedWidth = minimumWidth() == maximumWidth();
    bool isFixedHeight = minimumHeight() == maximumHeight();

    Utility::CornerEdge mouseCorner;
    QRect cornerRect;
    const QRect window_real_geometry = m_contentGeometry + QMargins(MOUSE_MARGINS, MOUSE_MARGINS, MOUSE_MARGINS, MOUSE_MARGINS);

    if (isFixedWidth || isFixedHeight)
        goto set_edge;

    /// begin set cursor corner type
    cornerRect.setSize(QSize(MOUSE_MARGINS * 2, MOUSE_MARGINS * 2));
    cornerRect.moveTopLeft(window_real_geometry.topLeft());

    if (cornerRect.contains(event->pos())) {
        mouseCorner = Utility::TopLeftCorner;

        goto set_cursor;
    }

    cornerRect.moveTopRight(window_real_geometry.topRight());

    if (cornerRect.contains(event->pos())) {
        mouseCorner = Utility::TopRightCorner;

        goto set_cursor;
    }

    cornerRect.moveBottomRight(window_real_geometry.bottomRight());

    if (cornerRect.contains(event->pos())) {
        mouseCorner = Utility::BottomRightCorner;

        goto set_cursor;
    }

    cornerRect.moveBottomLeft(window_real_geometry.bottomLeft());

    if (cornerRect.contains(event->pos())) {
        mouseCorner = Utility::BottomLeftCorner;

        goto set_cursor;
    }
set_edge:
    /// begin set cursor edge type
    if (event->x() <= m_contentGeometry.x()) {
        if (isFixedWidth)
            goto skip_set_cursor;

        mouseCorner = Utility::LeftEdge;
    } else if (event->x() < m_contentGeometry.right()) {
        if (isFixedHeight)
            goto skip_set_cursor;

        if (event->y() <= m_contentGeometry.y()) {
            mouseCorner = Utility::TopEdge;
        } else if (!isFixedWidth || event->y() >= m_contentGeometry.bottom()) {
            mouseCorner = Utility::BottomEdge;
        } else {
            goto skip_set_cursor;
        }
    } else if (!isFixedWidth && (!isFixedHeight || event->x() >= m_contentGeometry.right())) {
        mouseCorner = Utility::RightEdge;
    } else {
        goto skip_set_cursor;
    }
set_cursor:
    Utility::setWindowCursor(winId(), mouseCorner);

    if (qApp->mouseButtons() == Qt::LeftButton) {
        Utility::startWindowSystemResize(Utility::getNativeTopLevelWindow(winId()), mouseCorner);

        cancelAdsorbCursor();
    } else {
        adsorbCursor(mouseCorner);
    }

    return;
skip_set_cursor:
    qApp->setOverrideCursor(cursor());

    cancelAdsorbCursor();
    m_canAdsorbCursor = canResize();
}

void DFrameWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Utility::cancelWindowMoveResize(Utility::getNativeTopLevelWindow(winId()));

    return QRasterWindow::mouseReleaseEvent(event);
}

void DFrameWindow::resizeEvent(QResizeEvent *event)
{
    updateFrameMask();

    return QRasterWindow::resizeEvent(event);
}

bool DFrameWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Enter:
        m_canAdsorbCursor = canResize();
        break;
    case QEvent::Leave:
        m_canAdsorbCursor = false;
        cancelAdsorbCursor();
        break;
    default:
        break;
    }

    return QRasterWindow::event(event);
}

void DFrameWindow::setContentPath(const QPainterPath &path, bool isRoundedRect, int radius)
{
    if (m_clipPathOfContent == path)
        return;

    if (!isRoundedRect)
        m_contentGeometry = path.boundingRect().toRect().translated(contentOffsetHint());

    m_clipPathOfContent = path;

    if (isRoundedRect && m_pathIsRoundedRect == isRoundedRect && m_roundedRectRadius == radius && !m_shadowPixmap.isNull()) {
        const QMargins margins(qMax(m_shadowRadius + radius + qAbs(m_shadowOffset.x()), m_borderWidth),
                               qMax(m_shadowRadius + radius + qAbs(m_shadowOffset.y()), m_borderWidth),
                               qMax(m_shadowRadius + radius + qAbs(m_shadowOffset.x()), m_borderWidth),
                               qMax(m_shadowRadius + radius + qAbs(m_shadowOffset.y()), m_borderWidth));
        const QSize &margins_size = margins2Size(margins);

        if (margins_size.width() > m_contentGeometry.width() || margins_size.height() > m_contentGeometry.height()
                || margins_size.width() > m_shadowPixmap.width() || margins_size.height() > m_shadowPixmap.height()) {
            updateShadowPixmap();
        } else {
            m_shadowPixmap = QPixmap::fromImage(Utility::borderImage(m_shadowPixmap, margins * devicePixelRatio(),
                                                                     (m_contentGeometry + contentMarginsHint()).size() * devicePixelRatio()));
            m_shadowPixmap.setDevicePixelRatio(devicePixelRatio());
        }
    } else {
        m_pathIsRoundedRect = isRoundedRect;
        m_roundedRectRadius = radius;

        updateShadowPixmap();
    }

    updateMask();
}

void DFrameWindow::updateShadowPixmap()
{
    if (m_contentGeometry.isEmpty())
        return;

    int shadow_radius = qMax(m_shadowRadius, m_borderWidth);

    QImage image;

    if (shadow_radius > m_borderWidth) {
        QPixmap pixmap(m_contentGeometry.size() * devicePixelRatio());

        if (pixmap.isNull())
            return;

        pixmap.fill(Qt::transparent);

        QPainter pa(&pixmap);

        pa.fillPath(m_clipPathOfContent * devicePixelRatio(), m_shadowColor);
        pa.end();

        image = Utility::dropShadow(pixmap, shadow_radius * devicePixelRatio(), m_shadowColor);
        image.setDevicePixelRatio(devicePixelRatio());

        /// begin paint window border;

        pa.begin(&image);

        if (m_borderWidth > 0) {
            QPen pen;

            pen.setColor(m_borderColor);
            pen.setWidth(m_borderWidth * 2);
            pen.setJoinStyle(Qt::MiterJoin);

    //        pa.setCompositionMode(QPainter::CompositionMode_Source);
            pa.setPen(pen);
            pa.setRenderHint(QPainter::Antialiasing);
            pa.drawPath(m_clipPathOfContent.translated(contentOffsetHint()));
            pa.setRenderHint(QPainter::Antialiasing, false);
        }

        if (m_clearContent)
            pa.fillPath(m_clipPathOfContent.translated(QPoint(m_shadowRadius, m_shadowRadius) - m_shadowOffset), Qt::transparent);

        pa.end();
        /// end
    } else {
        image = QImage((m_contentGeometry + contentMarginsHint()).size() * devicePixelRatio(), QImage::Format_ARGB32_Premultiplied);
        image.fill(m_borderColor);
    }

    m_shadowPixmap = QPixmap::fromImage(image);
    m_shadowPixmap.setDevicePixelRatio(devicePixelRatio());
}

void DFrameWindow::updateContentMarginsHint()
{
    QMargins margins;

    margins = QMargins(qMax(m_shadowRadius - m_shadowOffset.x(), m_borderWidth),
                       qMax(m_shadowRadius - m_shadowOffset.y(), m_borderWidth),
                       qMax(m_shadowRadius + m_shadowOffset.x(), m_borderWidth),
                       qMax(m_shadowRadius + m_shadowOffset.y(), m_borderWidth));

    if (margins == m_contentMarginsHint)
        return;

    const QMargins old_margins = m_contentMarginsHint;

    m_contentMarginsHint = margins;
    m_contentGeometry.translate(m_contentMarginsHint.left() - old_margins.left(),
                                m_contentMarginsHint.top() - old_margins.top());

    updateShadowPixmap();
    update();

    if (isVisible()) {
        // Set frame extents
        Utility::setFrameExtents(winId(), margins * devicePixelRatio());
    }

    updateMask();

    emit contentMarginsHintChanged(old_margins);
}

void DFrameWindow::updateMask()
{
    // Set window clip mask
    int mouse_margins;

    if (DWMSupport::instance()->hasComposite())
        mouse_margins = canResize() ? MOUSE_MARGINS : 0;
    else
        mouse_margins = m_borderWidth;

    // clear old state
    Utility::setShapeRectangles(winId(), QRegion(), true);
    Utility::setShapeRectangles(winId(), QRegion(), false);

    if (m_enableAutoInputMaskByContentPath && (!m_pathIsRoundedRect || m_roundedRectRadius > 0)) {
        QPainterPath p;
        const QPainterPath &path = m_clipPathOfContent.translated(contentOffsetHint());

        if (Q_LIKELY(mouse_margins > 0)) {
            QPainterPathStroker stroker;
            stroker.setJoinStyle(Qt::MiterJoin);
            stroker.setWidth(mouse_margins * 2);
            p = stroker.createStroke(path);
            p = p.united(path);
            p.translate(-0.5, -0.5);
        } else {
            p = path;
        }

        Utility::setShapePath(winId(), p * devicePixelRatio(), DWMSupport::instance()->hasComposite());
    } else {
        QRegion region((m_contentGeometry.adjusted(-mouse_margins, -mouse_margins, mouse_margins, mouse_margins) * devicePixelRatio()).toRect());
        Utility::setShapeRectangles(winId(), region, DWMSupport::instance()->hasComposite());
    }

    updateFrameMask();
}

void DFrameWindow::updateFrameMask()
{
#ifdef Q_OS_LINUX
    QXcbWindow *xw = static_cast<QXcbWindow*>(handle());

    if (!xw || !xw->wmWindowTypes().testFlag(QXcbWindowFunctions::Dock))
        return;

    if (!m_enableAutoFrameMask || !DWMSupport::instance()->hasComposite())
        return;

    const QRect rect(QRect(QPoint(0, 0), size()));

    QRegion region((rect.united((m_contentGeometry + contentMarginsHint()))  * devicePixelRatio()).toRect());

    // ###(zccrs): xfwm4 window manager会自动给dock类型的窗口加上阴影， 所以在此裁掉窗口之外的内容
    setMask(region);
#endif
}

bool DFrameWindow::canResize() const
{
    bool ok = m_enableSystemResize
            && !flags().testFlag(Qt::Popup)
            && !flags().testFlag(Qt::BypassWindowManagerHint)
            && minimumSize() != maximumSize();

#ifdef Q_OS_LINUX
    if (!ok)
        return false;

    quint32 hints = DXcbWMSupport::getMWMFunctions(Utility::getNativeTopLevelWindow(winId()));

    return (hints == DXcbWMSupport::MWM_FUNC_ALL || hints & DXcbWMSupport::MWM_FUNC_RESIZE);
#endif

    return ok;
}

void DFrameWindow::cancelAdsorbCursor()
{
    QSignalBlocker blocker(&m_startAnimationTimer);
    Q_UNUSED(blocker)
    m_startAnimationTimer.stop();
    m_cursorAnimation.stop();
}

void DFrameWindow::adsorbCursor(Utility::CornerEdge cornerEdge)
{
    m_lastCornerEdge = cornerEdge;

    if (!m_canAdsorbCursor)
        return;

    if (m_cursorAnimation.state() == QVariantAnimation::Running)
        return;

    m_startAnimationTimer.start();
}

void DFrameWindow::startCursorAnimation()
{
    QPoint cursorPos = QCursor::pos();
    QPoint toPos = mapFromGlobal(cursorPos);
    const QRect geometry = m_contentGeometry.adjusted(-1, -1, 1, 1);

    switch (m_lastCornerEdge) {
    case Utility::TopLeftCorner:
        toPos = geometry.topLeft();
        break;
    case Utility::TopEdge:
        toPos.setY(geometry.y());
        break;
    case Utility::TopRightCorner:
        toPos = geometry.topRight();
        break;
    case Utility::RightEdge:
        toPos.setX(geometry.right());
        break;
    case Utility::BottomRightCorner:
        toPos = geometry.bottomRight();
        break;
    case Utility::BottomEdge:
        toPos.setY(geometry.bottom());
        break;
    case Utility::BottomLeftCorner:
        toPos = geometry.bottomLeft();
        break;
    case Utility::LeftEdge:
        toPos.setX(geometry.x());
        break;
    default:
        break;
    }

    toPos = mapToGlobal(toPos);
    const QPoint &tmp = toPos - cursorPos;

    if (qAbs(tmp.x()) < 3 && qAbs(tmp.y()) < 3)
        return;

    m_canAdsorbCursor = false;

    m_cursorAnimation.setStartValue(cursorPos);
    m_cursorAnimation.setEndValue(toPos);
    m_cursorAnimation.start();
}

DPP_END_NAMESPACE
